#include "inventory.h"
#include "sets.h"
// ==========================================================
// [1] 전역 변수 및 설정 데이터
// ==========================================================

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

// ==========================================================
// [2] 시스템 및 유틸리티 함수 (System & Utils)
// =========================================================

// 입력 버퍼 비우기
void clearBuffer() {
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}

// 카테고리별 출력 순서를 정해주는 보조 함수
int getCategoryOrder(const char* category) {
	for (int i = 0; i < NUM_CATEGORIES; i++) {
        	if (strcmp(category, VALID_CATEGORIES[i]) == 0) {
            	return i; // VALID_CATEGORIES 배열에 정의된 순서(0, 1, 2...)대로 정렬
        	}
    	}
    	return 999; // 혹시 모를 예외 카테고리는 맨 뒤로
}


// 데이터 파일 저장(F05-1)
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

// 데이터 파일 불러오기 (F05-2)
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

		if (head == NULL || getCategoryOrder(newNode->category) < getCategoryOrder(head->category)) {
            		newNode->next = head;
            		head = newNode;
		}
		else {
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

// 카테고리 메뉴 출력
void showCategoryMenu() {
    printf("\n---------------------------- [카테고리 선택] ----------------------------\n");
    for (int i = 0; i < NUM_CATEGORIES; i++) {
        printf("%2d. %-12s", i + 1, VALID_CATEGORIES[i]);
        if ((i + 1) % 5 == 0) printf("\n");
    }
    printf("\n-------------------------------------------------------------------------\n");
}

// 제조사 선택 메뉴 출력 (입력 보조)
void showManufacturerMenu() {
    printf("\n----------------------------- [제조사 선택] -----------------------------\n");
    for (int i = 0; i < NUM_MANUFACTURERS; i++) {
        printf("%2d. %-15s", i + 1, VALID_MANUFACTURERS[i]);
        if ((i + 1) % 4 == 0) printf("\n");
    }
    printf("\n-------------------------------------------------------------------------\n");
}

// ID로 특정 부품 검색
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

// 세트 출고 보조 함수
void processSetRelease(int ids[], int size, const char* setName) {
	// 1단계: 모든 부품이 존재하는지 및 재고 확인
	for (int i = 0; i < size ; i++) {
		Product* p = searchByID(ids[i]);
		if (p == NULL) {
			printf(">>[오류] %s 세트의 부품(ID:%d)이 리스트에 없습니다.\n", setName, ids[i]);
			return;
		}
		if (p->stock < 1) {
			printf(">> [품절] [%s] 부품 재고가 부족하여 세트 출고가 불가능합니다.\n", p->name);
            	return;
		}
	}
	// 2단계: 모든 부품이 준비되었을 때만 재고 차감
	printf("\n>> %s 출고를 시작합니다...\n", setName);
	for(int i=0; i<size; i++) {
		Product* p = searchByID(ids[i]);
		p->stock -= 1;
		printf("   - [%s] 재고 차감 완료 (남은 수량: %d)\n", p->name, p->stock);
    	}
	
	printf(">> [성공] %s 세트 1대가 정상 출고되었습니다!\n", setName);
	saveToFile();
}

// ==========================================================
// [3] 핵심 기능 (Core Features)
// ==========================================================

// F01: 신규 품목 등록 (Add)
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

	// 1. 리스트가 비어있거나, 새 제품이 첫 번째 노드보다 순서가 빠른 경우 (맨 앞에 삽입)
	if (head == NULL || getCategoryOrder(newNode->category) < getCategoryOrder(head->category)) {

        	newNode->next = head;
        	head = newNode;
    	} 

	// 2. 자기 자리를 찾아감 (카테고리 순서가 같거나 빠른 동안 전진)
    	else {
        	Product* current = head;
        	while (current->next != NULL && 
               		getCategoryOrder(current->next->category) <= getCategoryOrder(newNode->category)) {
            		current = current->next;
        	}
        // 찾은 위치 뒤에 끼워넣기
        newNode->next = current->next;
        current->next = newNode;
    	}

	printf(">> 등록 완료! (ID: %d, 제조사: %s, 제품명: %s)\n", 
		newNode->id, newNode->manufacturer, newNode->name);
	saveToFile();
}

//  F02: 품목 정보 수정
void updateProduct() {
    if (head == NULL) {
        printf("\n>> 수정할 재고가 없습니다.\n");
        return;
    }

    listProducts();

    int targetNo;
    printf("\n-------- 품목 정보 수정 --------\n");
    printf("\n수정할 부품의 번호(No.)를 입력하세요: ");
    if (scanf("%d", &targetNo) != 1) {
        printf("X 숫자로만 입력해주세요!\n");
        clearBuffer();
        return;
    }
    clearBuffer();

    //1. ID를 무시하고 리스트의 '순수 위치(No.)'로만 찾아감
    Product* target = head;
    int currentPos = 1;

    while (target != NULL && currentPos < targetNo) {
        target = target->next;
        currentPos++;
    }

    //2. 해당 번호에 노드가 없는 경우 (예: 리스트는 5번까진데 10번 입력)
    if (target == NULL) {
        printf("X 입력하신 번호(%d)에 해당하는 부품이 없습니다.\n", targetNo);
        return;
    }

    // 3. 찾았다면 그 위치의 데이터를 즉시 수정! (ID는 건드리지 않음)
    printf("\n>> [%s] %s 제품을 수정합니다.\n", target->manufacturer, target->name);
    printf("1. 매입가(원가) 수정 (현재: %d원)\n", target->costPrice);
    printf("2. 판매가(출고가) 수정 (현재: %d원)\n", target->sellPrice);
    printf("3. 재고 수량 수정 (현재: %d개)\n", target->stock);
    printf("0. 수정 취소\n");
    printf("선택 >> ");

    int upChoice;
    if (scanf("%d", &upChoice) != 1) {
        printf("❌ 숫자로만 입력해주세요!\n");
        clearBuffer();
        return;
    }
    clearBuffer();

    switch(upChoice) {
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
	saveToFile();
}

// F03: 품목 삭제 (Delete)
void deleteProduct() {
    if (head == NULL) { 
	printf("\n>> [오류] 삭제할 재고가 없습니다.\n"); 
	return; 
    }

    listProducts();

    int targetNo;
    printf("\n삭제할 항목의 번호(No.)를 입력하세요: ");
    scanf("%d", &targetNo); 
    clearBuffer();

    Product *curr = head, *prev = NULL;
    int count = 1;

    // targetNo번째 노드 찾기
    while (curr != NULL && count < targetNo) {
        prev = curr;
        curr = curr->next;
        count++;
    }

    if (curr == NULL) { 
	printf(">> [오류] 해당 번호의 항목을 찾을 수 없습니다.\n"); 
	return; 
    }

    char confirm;
    printf(">> [%s] %s 제품을 삭제하시겠습니까? (y/n): ", curr->manufacturer, curr->name);
    scanf(" %c", &confirm); clearBuffer();

    if (confirm == 'y' || confirm == 'Y') {
        if (prev == NULL) head = curr->next;
        else prev->next = curr->next;

        free(curr);
        printf(">> 삭제 완료!\n");
        saveToFile();
    }
}

// F04: 출고 관리 (Release)
void releaseProduct() {
	printf("\n--- [출고 관리] ---\n");
   	printf("1. 단품 출고 \n");
	printf("2. 세트 출고 \n");
	printf("0. 뒤로가기\n");
	printf("================================\n");
	printf("선택 >> ");
    
	int relChoise;
	if (scanf("%d", &relChoise) != 1) { 
		printf(">> [오류] 숫자로 입력해주세요.\n");
		clearBuffer(); 
		return;
	 }
	clearBuffer();

	if (relChoise == 0) return;
	
	if (relChoise == 1) {
		listProducts();
		int targetNo;
		printf("\n------- [단품 출고] -------\n");
        	printf("\n\n출고할 품목의 번호(No.)를 입력하세요: ");
		
		if (scanf("%d", &targetNo) != 1 ) {
			printf(">> [오류] 숫자로만 입력해주세요.\n");
            		clearBuffer();
            		return;
		}
		clearBuffer();

		Product* target = head;
		int currentPos = 1;
		
		while (target != NULL && currentPos < targetNo) {
            	target = target->next;
            	currentPos++;
        	}

		if (target == NULL || targetNo <= 0) {
			printf(">> [오류]  %d번에 해당하는 상품이 없습니다.\n", targetNo);
            		return;
		}

		printf(">> 선택된 상품: [%s] %s  %s (재고: %d개)\n", 
               	target->category,target->manufacturer, target->name, target->stock);

		int qty;
		printf("출고 수량을 입력하세요: ");
		if (scanf("%d",&qty) != 1 || qty <=0) {
			printf(">> [오류] 1 이상의 올바른 수량을 입력하세요.\n");
			clearBuffer();
			return;
		}
		clearBuffer();

		if (qty > target->stock) {
			printf(">> [거부] 재고가 부족합니다! (현재 재고: %d개)\n", target->stock);
		}
		else {
			target->stock -= qty;
            		printf(">> [성공] %d개 출고 완료! (남은 재고: %d개)\n", qty, target->stock);
            		saveToFile(); // 실시간 저장
		}
			
	}
	else if (relChoise == 2) {
		printf("\n[세트 카테고리 선택]\n");
        	printf("1. 사무/가정용 (B1~B6)\n");
        	printf("2. 게이밍/그래픽 작업 (G1~G6)\n");
        	printf("3. 고사양 게임/영상편집 (H1~H6)\n");
		printf("4. 프리미엄 4K (P1~P6)\n");
		printf("5. 딥러닝/워크스테이션 (W1~W6)\n");
        	printf("0. 취소\n");
		printf("======================================\n");
        	printf("선택 >> ");

		int setChoise;
		scanf("%d", &setChoise); 
		clearBuffer();
		
		//1. 사무/가정용 (B1~B6)
		if (setChoise == 1) {
			printf("\n--- [사무용 세트 목록] ---\n");
        		for (int i = 0; i < OFFICE_SETS_COUNT; i++) {
            			printf("%d. %s\n", i + 1, officeSets[i].setName);
       			}
        		printf("0. 취소\n선택 >> ");
        
     	   		int sub;
        		scanf("%d", &sub); 
			clearBuffer();
        
        		if (sub > 0 && sub <= OFFICE_SETS_COUNT) {
            			processSetRelease(officeSets[sub-1].ids, 
                                officeSets[sub-1].size, 
                                officeSets[sub-1].setName);
        		}
		}
		//2. 게이밍/그래픽 작업 (G1~G6)
		else if (setChoise == 2) {
			printf("\n--- [게이밍/그래픽 작업 세트 목록] ---\n");
                        for (int i = 0; i < GAMING_SETS_COUNT; i++) {
                                printf("%d. %s\n", i + 1, gamingSets[i].setName);
                        }
                        printf("0. 취소\n선택 >> ");

                        int sub;
                        scanf("%d", &sub);
                        clearBuffer();

                        if (sub > 0 && sub <= GAMING_SETS_COUNT) {
                                processSetRelease(gamingSets[sub-1].ids,
                                gamingSets[sub-1].size,
                                gamingSets[sub-1].setName);
                        }
		}
		//3. 고사양 게임/영상편집 (H1~H6)
		else if (setChoise == 3) {
		        printf("\n--- [고사양 게임/ 영상편집 세트 목록] ---\n");
                        for (int i = 0; i < PRO_SETS_COUNT; i++) {
                                printf("%d. %s\n", i + 1, proSets[i].setName);
                        }
                        printf("0. 취소\n선택 >> ");

                        int sub;
                        scanf("%d", &sub);
                        clearBuffer();

                        if (sub > 0 && sub <= PRO_SETS_COUNT) {
                                processSetRelease(proSets[sub-1].ids,
                                proSets[sub-1].size,
                                proSets[sub-1].setName);
                        }
		}
		//4. 프리미엄 4K (P1~P6)
		else if (setChoise == 4) {
			printf("\n--- [프리미엄 세트 목록] ---\n");
                        for (int i = 0; i < ULTRA_SETS_COUNT; i++) {
                                printf("%d. %s\n", i + 1, ultraSets[i].setName);
                        }
                        printf("0. 취소\n선택 >> ");

                        int sub;
                        scanf("%d", &sub);
                        clearBuffer();

                        if (sub > 0 && sub <= ULTRA_SETS_COUNT) {
                                processSetRelease(ultraSets[sub-1].ids,
                                ultraSets[sub-1].size,
                                ultraSets[sub-1].setName);
                        }
		}
		//5. 딥러닝/워크스테이션 (W1~W6}
		else if (setChoise == 5) {
			printf("\n--- [딥러닝/워크스테이션 세트 목록] ---\n");
                        for (int i = 0; i < WORK_SETS_COUNT; i++) {
                                printf("%d. %s\n", i + 1, workSets[i].setName);
                        }
                        printf("0. 취소\n선택 >> ");

                        int sub;
                        scanf("%d", &sub);
                        clearBuffer();

                        if (sub > 0 && sub <= WORK_SETS_COUNT) {
                                processSetRelease(workSets[sub-1].ids,
                                workSets[sub-1].size,
                                workSets[sub-1].setName);
                        }
		}
		else if (setChoise == 0) {
		clearBuffer();
		releaseProduct();
		}

		
	}
	else {
		printf(">> [오류] 잘못된 메뉴 선택입니다.\n");
		clearBuffer();
	}
}

// F05: 재고 현황 조회 (List)
void listProducts() {
	if(head == NULL) {
		printf("\n>>> 등록된 재고가 없습니다.\n");
		return;
	}

	printf("\n%s\n", "================================================================================================================");
	printf("%-4s\t%-10s\t%-15s\t%-25s\t%-10s\t%-10s\t%-6s\n", 
           	"No", "분류", "제조사", "제품명", "매입가", "판매가", "재고");
	printf("%s\n", "----------------------------------------------------------------------------------------------------------------");

	Product* current = head;
	int displayNo = 1; // 화면에만 보여줄 가짜 번호 (1번부터 시작)
	
	while (current != NULL) {
		int profit = current->sellPrice - current->costPrice;
		double marginRate = 0.0;

		if (current->costPrice > 0) {
			marginRate = ( (double)profit / current->costPrice)*100.0;
		}
	printf("%-4d\t%-10s\t%-15s\t%-25s\t%-10d\t%-10d\t%6d\n",
               displayNo++, 
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


