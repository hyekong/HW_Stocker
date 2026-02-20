#include "inventory.h"

Product* head = NULL;
int lastId = 0;

// 카테고리 배열
const char* VALID_CATEGORIES[] = {
	"CPU", "Cooler", "Motherboard", "Memory", "GPU", "HDD", "SSD", 
   	"ODD", "Expansion", "PSU", "Case", "OS", "Monitor",  
    	"Keyboard", "Mouse", "Audio", "Printer", "Router", "Peripherals"
};

const int NUM_CATEGORIES = 19;

// 제조사 배열
const char* VALID_MANUFACTURERS[] = {
    "Intel", "AMD", "Samsung", "SKhynix", "ASUS", 
    "MSI", "GIGABYTE", "Emtek", "Micron", "Seasonic", 
    "Zalman", "ABKO", "ETC.."
};

const int NUM_MANUFACTURERS = 13;

// 버퍼 비우기
void clearBuffer() {
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}

// 카테고리 메뉴 출력
void showCategoryMenu() {
    printf("\n---------------------------- [카테고리 선택] ----------------------------\n");
    for (int i = 0; i < NUM_CATEGORIES; i++) {
        printf("%2d. %-12s", i + 1, VALID_CATEGORIES[i]);
        if ((i + 1) % 5 == 0) printf("\n");
    }
    printf("\n-------------------------------------------------------------------------\n");
}

// 제조사 메뉴 출력
void showManufacturerMenu() {
    printf("\n----------------------------- [제조사 선택] -----------------------------\n");
    for (int i = 0; i < NUM_MANUFACTURERS; i++) {
        printf("%2d. %-15s", i + 1, VALID_MANUFACTURERS[i]);
        if ((i + 1) % 4 == 0) printf("\n");
    }
    printf("\n-------------------------------------------------------------------------\n");
}

// F01: 신규 품목 등록 (원가, 판매가 분리)
void addProduct() {
	Product* newNode = (Product*)malloc(sizeof(Product));
	if (!newNode) return;

	newNode->id = ++lastId;

	printf("\n--- 신규 부품 등록 ---\n");
	
	//(1) 카테고리 먼저 선택받기	
	int catChoice;
	while (1) {
		showCategoryMenu();
		printf("등록할 카테고리의 번호를 입력하세요 (1~%d): ", NUM_CATEGORIES);
		if ( scanf("%d", &catChoice) != 1) {
			printf("X 숫자로만 입력해주세요!\n");
			clearBuffer();
			continue;
		}
		clearBuffer();

		if (catChoice >= 1 && catChoice <= NUM_CATEGORIES) {
			strcpy(newNode->category, VALID_CATEGORIES[catChoice - 1]);
			printf(">> [%s] 카테고리가 선택되었습니다.\n\n", newNode->category);
			break;
		}
		else {
			printf("X 잘못된 번호입니다. 1에서 %d 사이의 번호를 입력해주세요.\n", NUM_CATEGORIES);
		}
	}

	//(2) 제조사 선택 받기
	int mfgChoice;
	while(1) {
		showManufacturerMenu();
		printf("제조사 번호를 선택하세요 (1~%d): ", NUM_MANUFACTURERS);
		if ( scanf("%d", &mfgChoice) != 1) {
			printf(" XX 숫자로만 입력해주세요!\n");
			clearBuffer();
			continue;
		}
		clearBuffer();	
	
		if(mfgChoice >= 1 && mfgChoice <= NUM_MANUFACTURERS) {
			// '기타' 선택 시 직접 입력
			if(mfgChoice == NUM_MANUFACTURERS) {
				printf("제조사 이름을 직접 입력하세요 (띄어쓰기 없이): ");
				scanf("%s", newNode->manufacturer);
				clearBuffer();
			}
			else {
				strcpy(newNode->manufacturer, VALID_MANUFACTURERS[mfgChoice - 1]);
			}
			printf(">> [%s] 제조사가 선택되었습니다.\n\n", newNode->manufacturer);
			break;
		}
		else {
		printf("XX 잘못된 번호입니다.\n");
		}
	}

	// (3) 나머지 정보 입력
	printf("제품명을 입력할 땐, 띄어쓰기 대신 '_' 를 사용해주세요.\n");
	printf("(예 : 인텔_코어i5)\n");
	printf("제품명: ");
	scanf("%s", newNode->name);
	clearBuffer();

	printf("매입가: ");
	scanf("%d", &newNode->costPrice);
	clearBuffer();

	printf("판매가: ");
	scanf("%d" , &newNode->sellPrice);
	clearBuffer();

	printf("초기 수량: ");
	scanf("%d", &newNode->stock);
	clearBuffer();

	newNode->next=NULL;

	if (head == NULL) {
		head = newNode;
	}
	else {
		Product* current = head;
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = newNode;
	}
	printf(">> 등록 완료! (ID: %d, 제조사: %s, 제품명: %s)\n", 
		newNode->id, newNode->manufacturer, newNode->name);
}

// F04: 목록 조회
void listProducts() {
	if(head == NULL) {
		printf("\n>>> 등록된 재고가 없습니다.\n");
	return;
	}

	printf("\n%s\n", "================================================================================================================");
	printf("%-4s\t%-10s\t%-15s\t%-25s\t%-10s\t%-10s\t%-6s\n", 
           	"ID", "분류", "제조사", "제품명", "매입가", "판매가", "재고");
	printf("%s\n", "----------------------------------------------------------------------------------------------------------------");

	Product* current = head;

	while (current != NULL) {
		int profit = current->sellPrice - current->costPrice;
		double marginRate = 0.0;

		if (current->costPrice > 0) {
			marginRate = ( (double)profit / current->costPrice)*100.0;
		}
	printf("%-4d\t%-10s\t%-15s\t%-25s\t%-10d\t%-10d\t%6d\n",
               current->id, 
               current->category, 
               current->manufacturer, 
               current->name, 
		current->costPrice,
               current->sellPrice, 
               current->stock);
        
        current = current->next;
    }
    printf("%s\n", "================================================================================================================");
}


// F05: 데이터 저장
void saveToFile() {
	FILE *fp = fopen("inventory.txt", "w");
	if (fp == NULL) return ;

	Product* current = head;
	while (current != NULL) {
		fprintf(fp, "%d %s %s %s %d %d %d\n", 
                	current->id, current->category, current->manufacturer , current->name, 
                	current->costPrice, current->sellPrice, current->stock);
		current = current->next;
	}
	fclose(fp);
	printf(">> 데이터가 파일에 자동 저장되었습니다.\n");
}

// F05: 데이터 불러오기
void loadFromFile() {
	FILE *fp = fopen("inventory.txt", "r");
	if (fp == NULL) {
		printf(">> 기존 데이터가 없습니다. 새로운 빈 창고로 시작합니다.\n");
        	return;
	}
	
	int id, cost, sell, stock;
	char category[MAX_CAT], manufacturer[MAX_MFG], name[MAX_NAME];
	int count = 0;
	
	while (fscanf(fp, "%d %s %s %s %d %d %d", &id, category, manufacturer, name, &cost, &sell, &stock) == 7) {
		Product* newNode = (Product*)malloc(sizeof(Product));
		newNode->id = id;
		strcpy(newNode->category, category);
		strcpy(newNode->manufacturer, manufacturer);
		strcpy(newNode->name, name);
		newNode->costPrice = cost;
		newNode->sellPrice = sell;
		newNode->stock = stock;
		newNode->next = NULL;

		if (head == NULL) {
			head = newNode;
		}
		else {
			Product* current = head;
			while (current->next != NULL) current = current->next;
			current->next = newNode;
		}

		if (id > lastId) lastId = id;
	}

	fclose(fp);
	printf(">> 저장된 데이터를 성공적으로 불러왔습니다.\n");

}

// 📌 유틸리티: ID로 부품 검색하기
Product* searchByID(int id) {
    Product* current = head;
    while (current != NULL) {
        if (current->id == id) {
            return current; // 찾으면 해당 부품의 메모리 주소 반환
        }
        current = current->next;
    }
    return NULL; // 리스트 끝까지 뒤져도 없으면 NULL 반환
}

//  F02: 품목 정보 수정
void updateProduct() {
    if (head == NULL) {
        printf("\n>> 수정할 재고가 없습니다.\n");
        return;
    }

    int targetId;
    printf("\n--- 품목 정보 수정 ---\n");
    // 사용자가 번호를 헷갈리지 않게 목록을 한 번 보여줍니다.
    listProducts(); 
    
    printf("\n수정할 부품의 ID를 입력하세요: ");
    if (scanf("%d", &targetId) != 1) {
        printf("X 숫자로만 입력해주세요!\n");
        clearBuffer();
        return;
    }
    clearBuffer();

    // 입력한 ID가 실제로 존재하는지 검색
    Product* target = searchByID(targetId);
    if (target == NULL) {
        printf("X 입력하신 ID(%d)에 해당하는 부품이 없습니다.\n", targetId);
        return;
    }

    // 부품을 찾았으면 무엇을 수정할지 서브 메뉴 출력
    printf("\n>> [%s] %s 제품을 수정합니다.\n", target->manufacturer, target->name);
    printf("1. 매입가(원가) 수정 (현재: %d원)\n", target->costPrice);
    printf("2. 판매가(출고가) 수정 (현재: %d원)\n", target->sellPrice);
    printf("3. 재고 수량 수정 (현재: %d개)\n", target->stock);
    printf("0. 수정 취소\n");
    printf("선택 >> ");

    int choice;
    if (scanf("%d", &choice) != 1) {
        printf("❌ 숫자로만 입력해주세요!\n");
        clearBuffer();
        return;
    }
    clearBuffer();

    switch(choice) {
        case 1:
            printf("새 매입가: ");
            scanf("%d", &target->costPrice);
            clearBuffer();
            printf(">> 매입가가 수정되었습니다.\n");
            break;
        case 2:
            printf("새 판매가: ");
            scanf("%d", &target->sellPrice);
            clearBuffer();
            printf(">> 판매가가 수정되었습니다.\n");
            break;
        case 3:
            printf("새 재고 수량: ");
            scanf("%d", &target->stock);
            clearBuffer();
            printf(">> 재고 수량이 수정되었습니다.\n");
            break;
        case 0:
            printf(">> 수정을 취소합니다.\n");
            break;
        default:
            printf("❌ 잘못된 선택입니다.\n");
    }
}


















