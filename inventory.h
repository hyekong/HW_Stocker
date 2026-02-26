#ifndef INVENTORY_H
#define INVENTORY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
extern CustomSet* customHead;

extern const char* VALID_CATEGORIES[];
extern const int NUM_CATEGORIES;

extern const char* VALID_MANUFACTURERS[];
extern const int NUM_MANUFACTURERS;


// --- Utility & System ---
void clearBuffer();
void saveToFile();
void loadFromFile();
void showCategoryMenu();     // 카테고리 메뉴 출력
void showManufacturerMenu(); // 제조사 메뉴 출력
Product* searchByID(int id); // ID로 부품 검색
Product* searchByNo(int no);  // 순번(No.)으로 부품 검색
int getCategoryOrder(const char* category); // 카테고리별 출력 순서 정렬

void saveCustomSet();			// 커스텀 세트 추가
void deleteCustomSet(char* targetName);	// 커스텀 세트 삭제
void loadCustomSetsFromFile();		// 커스텀 세트 파일 로드
void saveCustomSetsToFile();		// 커스텀 세트 파일 저장
void processSetRestock(int ids[], int size, int qty, const char* setName);	// 세트 입고
void processSetRelease(int ids[], int size, int qty, const char* setName);	// 세트 출고
void logSale(const char* productName, int qty, int cost, int sell);		// 장부 파일 로드

// --- Core Features (Match with Menu) ---
void addProduct();      // F01. 신규 품목 등록 (Add)
void updateProduct();   // F02. 품목 정보 수정 (Update)
void deleteProduct();   // F03. 품목 삭제 (Delete)
void releaseProduct();  // F04. 출고 관리 (Release)
void listProducts(int mode);    // F05. 재고 현황 조회 (List)
void viewSalesLog();	// F06.매출 및 장부 조회

#endif
