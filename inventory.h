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

// 커스텀 세트 구조체
typedef struct CustomSet {
	char setName[50];       // 세트 이름
	int ids[20];            // 포함된 부품 ID들
	int itemCount;          // 부품 종류 개수
	struct CustomSet* next; // 다음 세트를 가리키는 포인터
} CustomSet;

extern Product* head;
extern int lastId;

extern const char* VALID_CATEGORIES[];
extern const int NUM_CATEGORIES;

extern const char* VALID_MANUFACTURERS[];
extern const int NUM_MANUFACTURERS;


// --- Utility & System ---
void clearBuffer();
void saveToFile();
void loadFromFile();
void showCategoryMenu();     // 입력 보조
void showManufacturerMenu(); // 입력 보조
Product* searchByID(int id); // 내부 로직 보조
void saveCustomSet();
void loadCustomSetsFromFile();
void saveCustomSetsToFile();
void processSetRestock(int ids[], int size, int qty, const char* setName);
void processSetRelease(int ids[], int size, int qty, const char* setName);
// --- Core Features (Match with Menu) ---
void addProduct();      // F01. 신규 품목 등록 (Add)
void updateProduct();   // F02. 품목 정보 수정 (Update)
void deleteProduct();   // F03. 품목 삭제 (Delete)
void releaseProduct();  // F04. 출고 관리 (Release)
void listProducts(int mode);    // F05. 재고 현황 조회 (List)

#endif
