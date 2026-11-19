#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXV 100
#define MAXDAYS 30

struct Vehicle;

struct Slot {
    int id;
    int size;
    int occupied;
    struct Vehicle *vehicle;
};

struct Vehicle {
    char plate[20];
    int size;
    int entryTime;
    int inUse;
    struct Slot *slot;
};

struct Slot *slots;
int slotCount = 6;
struct Vehicle vehicles[MAXV];

int currentTime = 480;

int dayVehicles[MAXDAYS];
int dayRevenue[MAXDAYS];
int dayFines[MAXDAYS];

char *sizeName(int s)
{
    if (s == 1) return "Small";
    if (s == 2) return "Medium";
    return "Large";
}

void showTime(int t)
{
    printf("Day %d %02d:%02d", t / 1440 + 1, (t % 1440) / 60, t % 60);
}

void setupSlots()
{
    int i;
    slots = (struct Slot *) malloc(slotCount * sizeof(struct Slot));
    for (i = 0; i < slotCount; i++) {
        slots[i].id = i + 1;
        slots[i].size = i / 2 + 1;
        slots[i].occupied = 0;
        slots[i].vehicle = NULL;
    }
    for (i = 0; i < MAXV; i++)
        vehicles[i].inUse = 0;
}

struct Slot *findSlot(int vehicleSize)
{
    int s, i;
    for (s = vehicleSize; s <= 3; s++) {
        for (i = 0; i < slotCount; i++) {
            if (slots[i].occupied == 0 && slots[i].size == s)
                return &slots[i];
        }
    }
    return NULL;
}

struct Vehicle *findVehicle(char plate[])
{
    int i;
    for (i = 0; i < MAXV; i++) {
        if (vehicles[i].inUse == 1 && strcmp(vehicles[i].plate, plate) == 0)
            return &vehicles[i];
    }
    return NULL;
}

void parkVehicle()
{
    char plate[20];
    int size, i;
    struct Slot *s;

    printf("Enter plate number: ");
    scanf("%s", plate);
    if (findVehicle(plate) != NULL) {
        printf("This vehicle is already parked.\n");
        return;
    }
    printf("Vehicle size (1=Small 2=Medium 3=Large): ");
    scanf("%d", &size);
    if (size < 1 || size > 3) {
        printf("Invalid size.\n");
        return;
    }

    s = findSlot(size);
    if (s == NULL) {
        printf("No free slot available. Please expand the parking.\n");
        return;
    }

    for (i = 0; i < MAXV; i++) {
        if (vehicles[i].inUse == 0)
            break;
    }
    if (i == MAXV) {
        printf("Vehicle list is full.\n");
        return;
    }

    strcpy(vehicles[i].plate, plate);
    vehicles[i].size = size;
    vehicles[i].entryTime = currentTime;
    vehicles[i].inUse = 1;
    vehicles[i].slot = s;

    s->occupied = 1;
    s->vehicle = &vehicles[i];

    printf("Vehicle %s parked in slot %d (%s).\n", plate, s->id, sizeName(s->size));
}

int calculateFee(int entry, int exitTime, int size, int *fine)
{
    int minutes = exitTime - entry;
    int hours, i, hour, total = 0;

    if (minutes < 1)
        minutes = 1;
    hours = (minutes + 59) / 60;

    for (i = 0; i < hours; i++) {
        hour = ((entry + i * 60) / 60) % 24;
        if (hour >= 22 || hour < 6)
            total = total + 10;
        else
            total = total + 20;
    }

    total = total * size;

    *fine = 0;
    if (minutes > 480)
        *fine = ((minutes - 480 + 59) / 60) * 50;

    return total;
}

void exitVehicle()
{
    char plate[20];
    struct Vehicle *v;
    int fee, fine, day;

    printf("Enter plate number: ");
    scanf("%s", plate);
    v = findVehicle(plate);
    if (v == NULL) {
        printf("Vehicle not found.\n");
        return;
    }

    fee = calculateFee(v->entryTime, currentTime, v->size, &fine);

    printf("\n----- BILL -----\n");
    printf("Vehicle  : %s\n", v->plate);
    printf("Slot     : %d\n", v->slot->id);
    printf("Time     : %d minutes\n", currentTime - v->entryTime);
    printf("Fee      : Rs %d\n", fee);
    printf("Fine     : Rs %d\n", fine);
    printf("Total    : Rs %d\n", fee + fine);

    day = currentTime / 1440;
    if (day < MAXDAYS) {
        dayVehicles[day]++;
        dayRevenue[day] += fee;
        dayFines[day] += fine;
    }

    v->slot->occupied = 0;
    v->slot->vehicle = NULL;
    v->inUse = 0;
}

void searchVehicle()
{
    char plate[20];
    struct Vehicle *v;

    printf("Enter plate number: ");
    scanf("%s", plate);
    v = findVehicle(plate);
    if (v == NULL) {
        printf("Vehicle not found.\n");
        return;
    }
    printf("Vehicle %s is in slot %d, parked for %d minutes.\n",
           v->plate, v->slot->id, currentTime - v->entryTime);
}

void displaySlots()
{
    int i;
    printf("\nSlot  Size    Status    Vehicle\n");
    for (i = 0; i < slotCount; i++) {
        printf("%-5d %-7s ", slots[i].id, sizeName(slots[i].size));
        if (slots[i].occupied)
            printf("%-9s %s\n", "Occupied", slots[i].vehicle->plate);
        else
            printf("%-9s -\n", "Free");
    }
}

void advanceTime()
{
    int m;
    printf("Advance time by how many minutes? ");
    scanf("%d", &m);
    if (m > 0)
        currentTime = currentTime + m;
    printf("Current time: ");
    showTime(currentTime);
    printf("\n");
}

void expandParking()
{
    int extra, size, i;
    struct Slot *temp;

    printf("How many new slots? ");
    scanf("%d", &extra);
    printf("Size of new slots (1=Small 2=Medium 3=Large): ");
    scanf("%d", &size);
    if (extra < 1 || size < 1 || size > 3) {
        printf("Invalid input.\n");
        return;
    }

    temp = (struct Slot *) realloc(slots, (slotCount + extra) * sizeof(struct Slot));
    if (temp == NULL) {
        printf("Could not expand.\n");
        return;
    }
    slots = temp;

    for (i = slotCount; i < slotCount + extra; i++) {
        slots[i].id = i + 1;
        slots[i].size = size;
        slots[i].occupied = 0;
        slots[i].vehicle = NULL;
    }
    slotCount = slotCount + extra;

    for (i = 0; i < slotCount; i++) {
        if (slots[i].occupied == 1)
            slots[i].vehicle->slot = &slots[i];
    }

    printf("Parking expanded. Total slots = %d\n", slotCount);
}

void dailyReport()
{
    int day, i, occupied = 0;

    printf("Enter day number: ");
    scanf("%d", &day);
    if (day < 1 || day > MAXDAYS) {
        printf("Invalid day.\n");
        return;
    }

    for (i = 0; i < slotCount; i++) {
        if (slots[i].occupied == 1)
            occupied++;
    }

    printf("\n----- REPORT FOR DAY %d -----\n", day);
    printf("Vehicles billed : %d\n", dayVehicles[day - 1]);
    printf("Parking revenue : Rs %d\n", dayRevenue[day - 1]);
    printf("Fine revenue    : Rs %d\n", dayFines[day - 1]);
    printf("Total revenue   : Rs %d\n", dayRevenue[day - 1] + dayFines[day - 1]);
    printf("Occupancy now   : %d out of %d slots\n", occupied, slotCount);
}

int main()
{
    int choice;

    setupSlots();

    do {
        printf("\n===== SMART PARKING =====\n");
        printf("Time: ");
        showTime(currentTime);
        printf("\n");
        printf("1. Park vehicle\n");
        printf("2. Exit vehicle\n");
        printf("3. Search vehicle\n");
        printf("4. Display slots\n");
        printf("5. Advance time\n");
        printf("6. Expand parking\n");
        printf("7. Daily report\n");
        printf("0. Quit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: parkVehicle(); break;
            case 2: exitVehicle(); break;
            case 3: searchVehicle(); break;
            case 4: displaySlots(); break;
            case 5: advanceTime(); break;
            case 6: expandParking(); break;
            case 7: dailyReport(); break;
            case 0: printf("Goodbye!\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);

    free(slots);
    return 0;
}
