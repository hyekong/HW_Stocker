#ifndef INVENTORY_H
#define INVENTORY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 128
#define MAX_CAT 30
#define MAX_MFG 30

// 부품 노드 구조체
typedef struct Product {
    int id;
    char category[MAX_CAT];     // 카테고리
    char manufacturer[MAX_MFG]; // 제조사
    char name[MAX_NAME];        // 제품명
    int costPrice;              // 매입가
    int sellPrice;              // 판매가
    int stock;                  // 재고
    struct Product* next;
} Product;

extern Product* head;
extern int lastId;

extern const char* VALID_CATEGORIES[];
extern const int NUM_CATEGORIES;

extern const char* VALID_MANUFACTURERS[];
extern const int NUM_MANUFACTURERS;

void clearBuffer();
void showCategoryMenu();
void showManufacturerMenu();

void addProduct();
void listProducts();
void saveToFile();
void loadFromFile();

Product* searchByID(int id); // ID로 특정 부품 찾기
void updateProduct();        // F02: 부품 정보 수정 기능
void releaseProduct();       // 출고 기능
#endif
