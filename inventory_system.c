#include "inventory.h"

// ==========================================================
// [1] 전역 변수 및 설정 데이터
// ==========================================================

Product* head = NULL;
int lastId = 0;
CustomSet* customHead = NULL;

// 카테고리 데이터
const char* VALID_CATEGORIES[] = {
	"CPU", "Cooler", "Motherboard", "Memory", "GPU", "HDD", "SSD", 
   	"ODD", "Expansion", "PSU", "Case", "OS", "Monitor",  
    	"Keyboard", "Mouse", "Audio", "Printer", "Router", "Peripherals"
};

const int NUM_CATEGORIES = 19;

// 제조사 데이터
const char* VALID_MANUFACTURERS[] = {
    "Intel", "AMD", "Samsung", "SKhynix", "ASUS", 
    "MSI", "GIGABYTE", "Emtek", "Micron", "Seasonic", 
    "Zalman", "ABKO", "ETC.."
};

const int NUM_MANUFACTURERS = 13;

// ==========================================================
// [2] 시스템 및 유틸리티 함수 (System & Utils)
// ==========================================================

// 입력 버퍼 비우기
void clearBuffer() {
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}

// 카테고리 순서 정렬 보조
int getCategoryOrder(const char* category) {
	for (int i = 0; i < NUM_CATEGORIES; i++) {
        	if (strcmp(category, VALID_CATEGORIES[i]) == 0) {
            	return i; // VALID_CATEGORIES 배열에 정의된 순서(0, 1, 2...)대로 정렬
        	}
    	}
    	return 999; // 혹시 모를 예외 카테고리는 맨 뒤로
}

// ID로 부품 검색
Product* searchByID(int id) {
    Product* current = head;
    while (current != NULL) {
        if (current->id == id) return current;
        current = current->next;
    }
    return NULL;
}

// 순번(No.)으로 부품 검색
Product* searchByNo(int no) {
    if (no <= 0) return NULL;
    
    Product* current = head;
    int count = 1;
    
    while (current != NULL) {
        if (count == no) return current;
        current = current->next;
        count++;
    }
    return NULL; // 해당 순번이 없을 경우
}


// 데이터 파일 저장(F07-1)
void saveToFile() {
	FILE *fp = fopen("inventory.txt", "w");
	if (fp == NULL) {
	//[예외처리] 파일 열기 실패 시 안내 (권한 문제 등)
        printf(">> [치명적 오류] inventory.txt 파일에 저장할 수 없습니다!\n");
        return;
    }

	Product* current = head;
	while (current != NULL) {
		// 파이프(|) 기호로 데이터 분리
		fprintf(fp, "%d|%s|%s|%s|%d|%d|%d\n",
                	current->id, current->category, current->manufacturer , current->name, 
                	current->costPrice, current->sellPrice, current->stock);
		current = current->next;
	}
	fclose(fp);
	//printf(">> 데이터가 파일에 자동 저장되었습니다.\n");
}

// 데이터 파일 불러오기 (F07-2)
void loadFromFile() {
	FILE *fp = fopen("inventory.txt", "r");
	if (fp == NULL) {
		printf(">> 기존 데이터가 없습니다. 새로운 빈 창고로 시작합니다.\n");
        	return;
	}
	
	int id, cost, sell, stock;
	char category[MAX_CAT], manufacturer[MAX_MFG], name[MAX_NAME];
	
	// 파이프(|) 기호를 만날 때까지(%[^|]) 통째로 읽어오기 (띄어쓰기 호환)
	while (fscanf(fp, "%d|%[^|]|%[^|]|%[^|]|%d|%d|%d\n",
			 &id, category, manufacturer, name, &cost, &sell, &stock) == 7) {
		Product* newNode = (Product*)malloc(sizeof(Product));
		if (newNode == NULL) {
            	// [예외처리] 메모리 부족으로 터지는 현상 방지
            		printf(">> [오류] 시스템 메모리가 부족하여 일부 데이터를 불러오지 못했습니다.\n");
            		break; 
       		}

		newNode->id = id;
		strcpy(newNode->category, category);
		strcpy(newNode->manufacturer, manufacturer);
		strcpy(newNode->name, name);
		newNode->costPrice = cost;
		newNode->sellPrice = sell;
		newNode->stock = stock;
		newNode->next = NULL;

		if (head == NULL || getCategoryOrder(newNode->category) < getCategoryOrder(head->category)) {
            		newNode->next = head;
            		head = newNode;
		} else {
			Product* current = head;
			while (current->next != NULL&&
				getCategoryOrder(current->next->category) <= getCategoryOrder(newNode->category)) {
				current = current->next;
			}
			newNode->next = current->next;
            		current->next = newNode;
		}
		if (id > lastId) lastId = id;
	}

	fclose(fp);
	printf(">> 저장된 데이터를 성공적으로 불러왔습니다.\n");
}


