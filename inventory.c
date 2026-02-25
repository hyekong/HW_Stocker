#include "inventory.h"
#include "sets.h"
// ==========================================================
// [1] 전역 변수 및 설정 데이터
// ==========================================================

Product* head = NULL;
int lastId = 0;
CustomSet* customHead = NULL;

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
void processSetRelease(int ids[], int size,int qty, const char* setName) {
	printf("\n>> [%s] 세트 출고 처리를 시작합니다.\n", setName);

	// 1단계: 모든 부품의 재고가 충분한지 확인
	int isEnough = 1;
	for (int i = 0; i < size ; i++) {
		Product* p = searchByID(ids[i]);
		if (p == NULL) {
			printf(" - [경고] ID %d 부품을 찾을 수 없습니다.\n", ids[i]);
			isEnough = 0;
		} else if (p->stock < qty) {
			printf(" - [재고 부족] %s (현재 재고: %d개, 필요 수량: %d개)\n", p->name, p->stock, qty);
			isEnough = 0;
		}
	}
		
	// 2단계: 재고가 모두 충분할 때만 실제 출고 진행
	if (isEnough) {
		for (int i=0; i<size; i++) {
			Product* p = searchByID(ids[i]);
			p->stock -= qty;

			logSale(p->name, qty, p->costPrice, p->sellPrice); //장부기록

			printf(" - %s %d개 출고 완료 (남은 재고: %d개)\n", p->name, qty, p->stock);
		}
		saveToFile();
		printf(">>세트 출고 및 장부 기록이 완료되었습니다.\n");
	} else { 
		printf(">> 출고 취소: 재고가 부족한 부품이 있어 세트 출고를 진행할 수 없습니다.\n");
	}
}

// ==========================================
// [커스텀 세트 유틸리티 3대장]
// ==========================================

// 1. 커스텀 세트 파일에 영구 저장하기
void saveCustomSetsToFile() {
	FILE* f = fopen("custom_sets.txt", "w");
	if (f == NULL) return;

	CustomSet* curr = customHead;
	while (curr != NULL) {
		fprintf(f, "%s %d ", curr->setName, curr->itemCount);
		for (int i = 0; i < curr->itemCount; i++) {
			fprintf(f, "%d ", curr->ids[i]);
		}
		fprintf(f, "\n");
		curr = curr->next;
	}
	fclose(f);
}

// 2. 프로그램 켤 때 커스텀 세트 불러오기
void loadCustomSetsFromFile() {
	FILE* f = fopen("custom_sets.txt", "r");
	if (f == NULL) return; // 파일 없으면 그냥 넘어감

	while (1) {
		CustomSet* newNode = (CustomSet*)malloc(sizeof(CustomSet));
		if (newNode == NULL) break;

		if (fscanf(f, "%s %d", newNode->setName, &newNode->itemCount) == EOF) {
			free(newNode);
			break;
		}

		for (int i = 0; i < newNode->itemCount; i++) {
			fscanf(f, "%d", &newNode->ids[i]);
		}

		newNode->next = customHead;
		customHead = newNode;
	}
	fclose(f);
}

// 3. 새로운 커스텀 세트 레시피 만들기
void saveCustomSet() {
	CustomSet* newNode = (CustomSet*)malloc(sizeof(CustomSet));
	if (newNode == NULL) return;

	printf("\n--- [새 커스텀 세트 만들기] ---\n");
	printf("세트 이름 (공백 없이): ");
	scanf("%s", newNode->setName);
	clearBuffer();

	int count = 0;
	int inputId;

	printf("\n포함할 부품의 ID를 하나씩 입력하세요. (종료: 0)\n");

	while (count < 20) {
		printf("[%d번째 부품] ID 입력 >> ", count + 1);
		scanf("%d", &inputId);
		clearBuffer();

		if (inputId == 0) break; // 0 누르면 입력 종료!

		if (searchByID(inputId) != NULL) {
			newNode->ids[count] = inputId;
			count++;
			printf(">> ID %d번 부품이 담겼습니다.\n", inputId);
		} else {
			printf(">> [경고] 존재하지 않는 부품입니다.\n");
		}
	}

	if (count > 0) {
		newNode->itemCount = count;
		newNode->next = customHead;
		customHead = newNode;

		saveCustomSetsToFile(); 
		printf("\n>> 성공띠! '%s' 세트(부품 %d종) 등록 완료!\n", newNode->setName, count);
	} else {
		printf("\n>> 담은 부품이 없어 취소되었습니다.\n");
		free(newNode);
	}
}
// 4. 커스텀 세트 삭제하기
void deleteCustomSet(char* targetName) {
    if (customHead == NULL) {
        printf(">> 등록된 커스텀 세트가 없습니다.\n");
        return;
    }

    CustomSet* curr = customHead;
    CustomSet* prev = NULL;

    // 1. 삭제할 세트 찾기
    while (curr != NULL && strcmp(curr->setName, targetName) != 0) {
        prev = curr;
        curr = curr->next;
    }

    // 2. 못 찾았을 경우
    if (curr == NULL) {
        printf(">> '%s' 세트를 찾을 수 없습니다.\n", targetName);
        return;
    }

    // 3. 연결 끊기
    if (prev == NULL) { // 맨 앞 노드를 지울 때
        customHead = curr->next;
    } else {
        prev->next = curr->next;
    }

    free(curr); // 메모리 해제
    saveCustomSetsToFile(); // 파일에 바로 저장
    printf(">> '%s' 커스텀 세트가 삭제되었습니다.\n", targetName);
}


// [유틸리티] 세트에 포함된 부품들의 재고를 일괄 증가(입고)시키는 함수
void processSetRestock(int ids[], int size, int qty, const char* setName) {
	printf("\n>> [%s] 세트 구성품 입고 처리를 시작합니다.\n", setName);
	
	for (int i = 0; i < size; i++) {
		Product* p = searchByID(ids[i]); // 부품 찾기
		if (p != NULL) {
			p->stock += qty; // 찾은 부품의 재고를 올려줌
			printf(" - %s (ID: %d) 재고 %d개 증가 (현재: %d개)\n", p->name, p->id, qty, p->stock);
		} else {
			printf(" - [경고] ID %d번 부품을 찾을 수 없습니다.\n", ids[i]);
		}
	}
	
	saveToFile(); // 재고가 바뀌었으니 파일에 즉시 저장! (함수 이름이 다르면 혜정님 코드로 맞춰주세요)
	printf(">> 입고 처리가 완료되었습니다.\n");
}

void logSale(const char* productName, int qty, int cost, int sell) {
	FILE* f = fopen("sales.txt", "a");	
	
	if (f == NULL) {
		printf(">> [오류] 장부 파일을 열 수 없습니다.\n");
		return;
	}

	// 1. 현재 시간 가져오기
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	// 2. 순이익 계산 공식
	// $NetProfit = (SellPrice - CostPrice) \times Quantity$
	int profit = (sell - cost) * qty;

	// 3. 파일에 예쁘게 기록 (날짜 | 제품명 | 출고수량 | 순이익)
	fprintf(f, "[%d-%02d-%02d] %-20s | 출고: %d개 | 순이익: %d원\n",
	        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
	        productName, qty, profit);

	fclose(f);
}

// ==========================================================
// [3] 핵심 기능 (Core Features)
// ==========================================================

// F01: 신규/기존 부품 입고(Add)
void addProduct() {
	printf("\n===== [입고 관리] =====\n");
	printf("1. 신규 부품 단일 입고\n");
	printf("2. 기존 부품 단일 입고\n");
	printf("3. 신규 커스텀 세트 입고\n");
	printf("4. 기존 커스텀 세트 입고\n");
	printf("5. 프리셋 세트 입고\n");
	printf("0. 뒤로가기\n");
	printf("선택 >> ");

	int addChoice;
	scanf("%d", &addChoice);
	clearBuffer();

	//F01-1.신규 부품 입고
	if (addChoice == 1) {
		printf("\n--- [1. 신규 부품 입고] ---\n");
		Product* newNode = (Product*)malloc(sizeof(Product));
		if (!newNode) return;

		newNode->id = ++lastId;

		//F01-1-1.신규 카테고리 선택
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
        	        } else {
                        	printf("X 잘못된 번호입니다. 1에서 %d 사이의 번호를 입력해주세요.\n", NUM_CATEGORIES);
                	}
        	}
		
		//F01-1-2.신규 제조사 선택
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
                        	} else {
                                strcpy(newNode->manufacturer, VALID_MANUFACTURERS[mfgChoice - 1]);
                        	}
                        	printf(">> [%s] 제조사가 선택되었습니다.\n\n", newNode->manufacturer);
                        	break;
                	} else {
                		printf("XX 잘못된 번호입니다.\n");
                	}
        	}
		
		//F01-1-3. 신규 나머지 정보 입력
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

		//F01-1-4.1. 리스트가 비어있거나, 새 제품이 첫 번째 노드보다 순서가 빠른 경우 (맨 앞에 삽입)
		if (head == NULL || getCategoryOrder(newNode->category) < getCategoryOrder(head->category)) {
			newNode->next = head;
			head = newNode;
		}
		//F01-1-4.2. 자기 자리를 찾아감 (카테고리 순서가 같거나 빠른 동안 전진)
        	else {
			Product* current = head;
			while (current->next != NULL &&getCategoryOrder(current->next->category) 
				<=getCategoryOrder(newNode->category)) {
					current = current->next;
                	}
		//F01-1-4.3.찾은 위치 뒤에 끼워넣기
        	newNode->next = current->next;
        	current->next = newNode;
        	}

		printf(">> 등록 완료! (ID: %d, 제조사: %s, 제품명: %s)\n",
                newNode->id, newNode->manufacturer, newNode->name);
        	saveToFile();
	}

        //F01-2. 기존 부품 입고
        else if (addChoice == 2) {
		listProducts(1);		
		printf("\n--- [2. 기존 부품 입고] ---\n");
		printf("입고할 부품의 ID를 입력하세요: ");
		
		int searchId;
		scanf("%d", &searchId);
		clearBuffer();

		// 입력한 ID로 부품 찾기 (어제 만든 만능 탐색기!)
		Product* p = searchByID(searchId);
		
		if (p != NULL) {
			// 부품을 찾았을 때
			printf("\n>> [%s] 부품을 찾았습니다. (현재 재고: %d개)\n", p->name, p->stock);
			printf("추가로 입고할 수량을 입력하세요 (취소: 0): ");
			
			int addQty;
			scanf("%d", &addQty);
			clearBuffer();

			if (addQty > 0) {
				p->stock += addQty; //  핵심: 기존 재고에 더하기
				saveToFile();       //  잊지 말고 장부에 기록 (함수 이름 맞는지 확인!)
				printf(">> 성공! %d개가 추가 입고되어 총 %d개가 되었습니다.\n", addQty, p->stock);
			} else if (addQty == 0) {
				printf(">> 입고가 취소되었습니다.\n");
			} else {
				printf(">> [오류] 올바르지 않은 수량입니다.\n");
			}
		} else {
			// 부품을 못 찾았을 때
			printf(">> [오류] 해당 ID(%d)를 가진 부품이 등록되어 있지 않습니다.\n", searchId);
		}
	}

	//F01-3. 신규 커스텀 세트 입고
        else if (addChoice == 3) {
		printf("\n--- [3. 신규 커스텀 세트 입고] ---\n");
		saveCustomSet(); 
		
		if (customHead != NULL) {
			printf("\n방금 만든 '%s' 세트를 총 몇 대 입고하시겠습니까? (취소: 0): ", customHead->setName);
			int restockQty;
			scanf("%d", &restockQty);
			clearBuffer();
			
			if (restockQty > 0) {
				processSetRestock(customHead->ids, customHead->itemCount, restockQty, customHead->setName);
			}
		}
        }

	//F01-4. 기존 커스텀 세트 입고
        else if (addChoice == 4) {
		printf("\n--- [4. 기존 커스텀 세트 입고] ---\n");
		if (customHead == NULL) {
			printf(">> [안내] 아직 등록된 커스텀 세트가 없습니다.\n");
		} else {
			CustomSet* curr = customHead;
			int index = 1;
			
			// 등록된 커스텀 세트 목록 출력
			while (curr != NULL) {
				printf("%d. %s (포함 부품 %d종)\n", index++, curr->setName, curr->itemCount);
				curr = curr->next;
			}
			
			printf("입고할 세트 번호 선택 (취소: 0) >> ");
			int selectNum;
			scanf("%d", &selectNum);
			clearBuffer();
			
			if (selectNum > 0 && selectNum < index) {
				// 사용자가 선택한 번호의 세트 찾기
				curr = customHead;
				for (int i = 1; i < selectNum; i++) curr = curr->next;
				
				// 찾은 세트로 실제 재고 더하기
				printf("\n>> '%s' 세트를 선택하셨습니다.\n", curr->setName);
				printf("입고할 수량(세트 단위) 입력: ");
				int restockQty;
				scanf("%d", &restockQty);
				clearBuffer();
				
				if (restockQty > 0) {
					processSetRestock(curr->ids, curr->itemCount, restockQty, curr->setName);
				}
			} else if (selectNum != 0) {
				printf(">> [오류] 잘못된 번호입니다.\n");
			}
		}
	}

	//F01-5. 프리셋 세트 입고
        else if (addChoice == 5) {
		printf("\n----- [5. 프리셋 세트 입고] -----\n");
		printf("1. 사무/가정용 세트 (B1~B6)\n");
		printf("2. 게이밍/그래픽 작업 세트 (G1~G6)\n");
		printf("3. 고사양 게임/영상편집 세트 (H1~H6)\n");
		printf("4. 프리미엄 4K 세트 (P1~P6)\n");
		printf("5. 딥러닝/워크스테이션 세트 (W1~W6)\n");
		printf("0. 취소\n");
		printf("카테고리 선택 >> ");
		
		int setCatChoice;
		scanf("%d", &setCatChoice);
		clearBuffer();

		//F01-5-1. 사무/가정용 세트 입고
		if (setCatChoice == 1) {
			printf("\n===== [사무/가정용 세트 선택] =====\n");
			printf("1. B1: G5905/내장 \n2. B2: 3200G/내장 \n3. B3: 13100F/GT710 \n4. B4: 8500G/내장 \n5. B5: 5600G/내장 \n6. B6: 12400/내장\n");
			printf("선택 >> ");

			int sub;
			scanf("%d", &sub);
			clearBuffer();
			
			if (sub >= 1 && sub <= 6) {
				printf("입고할 수량(세트 단위): ");
				int setRestockQty;
				scanf("%d", &setRestockQty);
				clearBuffer();
				
				if (setRestockQty > 0) {
					processSetRestock(officeSets[sub-1].ids, 
					                  officeSets[sub-1].size, 
					                  setRestockQty, 
					                  officeSets[sub-1].setName);
				}
			}
		}
		
		//F01-5-2. 게이밍/그래픽 작업 세트 입고
                else if (setCatChoice == 2) {
                        printf("\n===== [게이밍/그래픽 작업 세트 선택] =====\n");
			printf("1. G1: 245K/내장 \n2. G2: 5600/RTX3050 \n3. G3: 5600/RX7600 \n4. G4: 12400F/RTX5050 \n5. G5: 5600/RTX5060 \n6. G6: 14400F/RTX5060\n");
                        printf("선택 >> ");

                        int sub;
                        scanf("%d", &sub);
                        clearBuffer();

                        if (sub >= 1 && sub <= 6) {
                                printf("입고할 수량(세트 단위): ");
                                int setRestockQty;
                                scanf("%d", &setRestockQty);
                                clearBuffer();

                                if (setRestockQty > 0) {
                                        processSetRestock(gamingSets[sub-1].ids,
                                                          gamingSets[sub-1].size,
                                                          setRestockQty,
                                                          gamingSets[sub-1].setName);
                                }
                        }
                }

		//F01-5-3. 고사양 게임/영상편집 세트 입고
                else if (setCatChoice == 3) {
                        printf("\n===== [ 고사양 게임/영상편집 세트 선택] =====\n");
			printf("1. H1: 7400F/RTX5060 \n2. H2: 7400F/RX9060 XT \n3. H3: 9600/RX9060 XT \n4. H4: 7400/RTX5060 Ti \n5. H5: 14600KF/RTX5060 \n6. H6: 9600/RTX5060 Ti\n");
                        printf("선택 >> ");

                        int sub;
                        scanf("%d", &sub);
                        clearBuffer();

                        if (sub >= 1 && sub <= 6) {
                                printf("입고할 수량(세트 단위): ");
                                int setRestockQty;
                                scanf("%d", &setRestockQty);
                                clearBuffer();

                                if (setRestockQty > 0) {
                                        processSetRestock(proSets[sub-1].ids,
                                                          proSets[sub-1].size,
                                                          setRestockQty,
                                                          proSets[sub-1].setName);
                                }
                        }
                }

                //F01-5-4. 프리미엄 4K 세트 입고
                else if (setCatChoice == 4) {
                        printf("\n===== [ 프리미엄 4K 세트 선택] =====\n");
			printf("1. P1: 14600KF/RTX5070 \n2. P2: 9600/RTX5070 \n3. P3: 265KF/RTX5070 \n4. P4: 7800X3D/RX9070 XT \n5. P5: 7800X3D/RTX5070 Ti \n6. P6: 9800X3D/RTX5070 Ti\n");
                        printf("선택 >> ");

                        int sub;
                        scanf("%d", &sub);
                        clearBuffer();

                        if (sub >= 1 && sub <= 6) { 
                                printf("입고할 수량(세트 단위): ");
                                int setRestockQty;     
                                scanf("%d", &setRestockQty);
                                clearBuffer();

                                if (setRestockQty > 0) {
                                        processSetRestock(ultraSets[sub-1].ids,
                                                          ultraSets[sub-1].size,
                                                          setRestockQty,
                                                          ultraSets[sub-1].setName);
                                }
                        }
                }

		//F01-5-5. 딥러닝/워크스테이션 세트 입고
                else if (setCatChoice == 4) {
                        printf("\n===== [ 딥러닝/워크스테이션 세트 선택] =====\n");
			printf("1. W1: 7800X3D/RTX5080 \n2. W2: 265KF/RTX5080 \n3. W3: 9800X3D/RTX5080 \n4. W4: 14900KF/RTX5080 \n5. W5: 9900XRTX5090 \n6. W6: 285K/RTX5090\n");
                        printf("선택 >> ");

                        int sub;
                        scanf("%d", &sub);
                        clearBuffer();

                        if (sub >= 1 && sub <= 6) {
                                printf("입고할 수량(세트 단위): ");
                                int setRestockQty;
                                scanf("%d", &setRestockQty);
                                clearBuffer();

                                if (setRestockQty > 0) {
                                        processSetRestock(workSets[sub-1].ids,
                                                          workSets[sub-1].size,
                                                          setRestockQty,
                                                          workSets[sub-1].setName);
                                }
                        }
                }
		else if (setCatChoice == 0) {
			printf(">> 입고 메뉴 선택이 취소되었습니다.\n");
			return;
		}

	}

	else if (addChoice == 0) {
		return; // 뒤로가기
	}

	else {
		printf(">> [오류] 잘못된 입력입니다.\n");
	}

}
//  F02: 품목 정보 수정
void updateProduct() {
    if (head == NULL) {
        printf("\n>> 수정할 재고가 없습니다.\n");
        return;
    }

    listProducts(0);

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

	printf("\n===== [ 품목 삭제 ] =====\n");
        printf("1. 단일 품목 삭제\n");
        printf("2. 커스텀 세트 삭제\n");
        printf("0. 뒤로가기\n");
        printf("선택 >> ");
	
	int delChoice;
	scanf("%d",&delChoice);
	clearBuffer();
	
	if (delChoice == 1) {
		listProducts(0);

		int targetNo;
		printf("\n------ 1. 단일 품목 삭제 ------");
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
			if (prev == NULL) { head = curr->next;}
			else {prev->next = curr->next;}
		free(curr);
		printf(">> 삭제 완료!\n");
		saveToFile();	
		}
	} else if (delChoice == 2) {
		printf("\n----- [2. 커스텀 세트 삭제] -----\n");	

		// 세트가 아예 없을 때
		if (customHead == NULL) {
			printf(">> [안내] 등록된 커스텀 세트가 없습니다.\n");
			return; // 함수 빠져나가기
		}

		//커스텀 세트 목록 출력
		CustomSet* curr = customHead;
		int index = 1;
		while (curr != NULL) {
			printf("%d. %s (포함 부품 %d종)\n", index++, curr->setName, curr->itemCount);
			curr = curr->next;
		}

		//삭제할 번호 입력받기
		printf("0. 취소\n");
		printf("삭제할 세트 번호 선택 (취소 시 0) >> ");
		int delSetChoice;
		scanf("%d", &delSetChoice);
		clearBuffer();

		// 4. 취소 로직
		if (delSetChoice == 0) {
			printf(">> 삭제가 취소되었습니다.\n");
		}

		//5. 번호 유효시 진짜 삭제
		else if (delSetChoice > 0 && delSetChoice < index) {
			curr = customHead;
			CustomSet* prev = NULL;

			for (int i = 1; i < delSetChoice; i++) {
				prev = curr;
				curr = curr->next;
			}

			if (prev == NULL) {
				// 맨 앞칸(1번)을 지울 때: 머리를 다음 칸으로 옮김
				customHead = curr->next;
			} else {
				// 중간이나 끝칸을 지울 때: 내 앞칸과 내 뒷칸을 연결해버림 
				prev->next = curr->next;
			}
			
			printf(">> '%s' 커스텀 세트가 삭제되었습니다.\n", curr->setName);

			// 메모리에서 완전히 삭제
			free(curr);

			//지워진 결과를 custom_sets.txt 파일에 덮어쓰기
			saveCustomSetsToFile();
		}
		
	} else if (delChoice == 0) {
		return; 
	} else {
		printf(">> [오류] 잘못된 번호입니다.\n");
	}	
  
}

// F04: 출고 관리 (Release)
void releaseProduct() {
	printf("\n--- [출고 관리] ---\n");
   	printf("1. 단품 출고 \n");
	printf("2. 커스텀 세트 출고 \n");
	printf("3. 프리셋 세트 출고 \n");
	printf("0. 뒤로가기\n");
	printf("================================\n");
	printf("선택 >> ");

	int relQty = 0;
	int relChoise;
	if (scanf("%d", &relChoise) != 1) { 
		printf(">> [오류] 숫자로 입력해주세요.\n");
		clearBuffer(); 
		return;
	 }
	clearBuffer();

	if (relChoise == 1) {
		listProducts(0);
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

		printf("출고 수량을 입력하세요: ");
		if (scanf("%d",&relQty) != 1 || relQty <=0) {
			printf(">> [오류] 1 이상의 올바른 수량을 입력하세요.\n");
			clearBuffer();
			return;
		}
		clearBuffer();

		if (relQty > target->stock) {
			printf(">> [거부] 재고가 부족합니다! (현재 재고: %d개)\n", target->stock);
		}
		else {
			target->stock -= relQty;

			//장부기록
			logSale(target->name, relQty, target->costPrice, target->sellPrice);

            		printf(">> [성공] %d개 출고 완료! (남은 재고: %d개)\n", relQty, target->stock);
            		saveToFile(); // 실시간 저장
		}
			
	}
	else if (relChoise == 2) {
		printf("\n------- [ 커스텀 세트 출고 ] -------\n");
		if (customHead == NULL) {
			printf(">> [안내] 등록된 커스텀 세트가 없습니다.\n");
		} else {
			int index = 1;
			CustomSet* curr = customHead;
			while (curr != NULL) {
				printf("%d. %s (포함 부품 %d종)\n", index++, curr->setName, curr->itemCount);
				curr = curr->next;
			}
			
			printf("출고할 세트 번호 선택 (취소: 0) >> ");
			int selectNum;
			scanf("%d", &selectNum);
			clearBuffer();
			
			if (selectNum > 0 && selectNum < index) {
				curr = customHead;
				for (int i = 1; i < selectNum; i++) curr = curr->next;
				
				printf("\n>> '%s' 세트를 선택하셨습니다.\n", curr->setName);
				printf("출고할 수량(세트 단위) 입력: ");
				scanf("%d", &relQty);
				clearBuffer();
				
				if (relQty > 0) {
					// 입고(Restock) 대신 출고(Release) 함수 호출!
					processSetRelease(curr->ids, curr->itemCount, relQty, curr->setName);
				}
			} else if (selectNum != 0) {
				printf(">> [오류] 잘못된 번호입니다.\n");
			}
		}
	}

	else if (relChoise == 3) {
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
				printf("출고할 '%s' 세트의 수량을 입력하세요: ", officeSets[sub-1].setName);
		                scanf("%d", &relQty); 
                		clearBuffer();

            			processSetRelease(officeSets[sub-1].ids, 
                                officeSets[sub-1].size, 
				relQty,
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
				printf("출고할 '%s' 세트의 수량을 입력하세요: ", gamingSets[sub-1].setName);
                     		scanf("%d", &relQty); 
                     		clearBuffer();

                                processSetRelease(gamingSets[sub-1].ids,
                                gamingSets[sub-1].size,
                                relQty,
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
				printf("출고할 '%s' 세트의 수량을 입력하세요: ", proSets[sub-1].setName);
				scanf("%d", &relQty); 
				clearBuffer();

                                processSetRelease(proSets[sub-1].ids,
                                proSets[sub-1].size,
                                relQty,
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
				printf("출고할 '%s' 세트의 수량을 입력하세요: ", ultraSets[sub-1].setName);
				scanf("%d", &relQty); 
				clearBuffer();

                                processSetRelease(ultraSets[sub-1].ids,
                                ultraSets[sub-1].size,
                                relQty,
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
				printf("출고할 '%s' 세트의 수량을 입력하세요: ", workSets[sub-1].setName);
				scanf("%d", &relQty); 
				clearBuffer();

                                processSetRelease(workSets[sub-1].ids,
                                workSets[sub-1].size,
                                relQty,
                                workSets[sub-1].setName);
                        }
		}
		else if (setChoise == 0) {
		clearBuffer();
		releaseProduct();
		}
		else if (relChoise == 0) return;
	}
	else {
		printf(">> [오류] 잘못된 메뉴 선택입니다.\n");
		clearBuffer();
	}
}

// F05: 재고 현황 조회 (List) mode가 0이면 일반(NO만), 1이면 입고용(NO + ID)
void listProducts(int mode) {
	if(head == NULL) {
		printf("\n>>> 등록된 부품이 없습니다.\n");
		return;
	}

	//ID 출력
	if(mode==1) {
		printf("===============================================================================================================================\n");
		printf("%-4s\t%-10s\t%-15s\t%-30s\t%-15s\t%-17s\t%-6s\n", 
           		"ID", "분류", "제조사", "제품명", "매입가", "판매가", "재고");
		printf("-------------------------------------------------------------------------------------------------------------------------------\n");
	} else {
		printf("====================================================================================================================\n");
                printf("%-4s\t%-10s\t%-15s\t%-30s\t%-15s\t%-17s\t%-6s\n",
                        "No", "분류", "제조사", "제품명", "매입가", "판매가", "재고");
                printf("--------------------------------------------------------------------------------------------------------------------\n");
	}

	Product* curr = head;
	int no = 1;
	while (curr != NULL) {
		if (mode == 1) {
			// 입고용: ID 보여줌
			printf("%-4d\t%-10s\t%-15s\t%-25s\t%-10d\t%-9d\t%6d\n",
				curr->id,curr->category,curr->manufacturer,curr->name,curr->costPrice,curr->sellPrice,curr->stock);
		} else {
			// 일반용: NO만 보여줌
			printf("%-4d\t%-10s\t%-15s\t%-25s\t%-10d\t%-9d\t%6d\n",
                        	no,curr->category,curr->manufacturer,curr->name,curr->costPrice,curr->sellPrice,curr->stock);		
		}
		
		curr = curr->next;
		no++;
	}
	printf("============================================================================================================================\n");	
} 

//F06. 장부 조회(sales.txt 파일의 내용을 화면에 출력)
void viewSalesLog() {
	printf("\n===========================================================\n");
	printf("                    💰 매출 및 장부 조회 💰                    \n");
	printf("===========================================================\n");

	// "r" 모드: 읽기 전용으로 열기!
	FILE* f = fopen("sales.txt", "r"); 
	if (f == NULL) {
		printf(">> [안내] 아직 판매 내역이 없습니다. (첫 개시를 기다립니다!)\n");
		printf("===========================================================\n");
		return;
	}

	char buffer[256]; // 한 줄씩 읽어올 임시 바구니
	int count = 0;    // 거래 건수 세기

	// 파일의 끝(EOF)에 도달할 때까지 한 줄씩 읽어서 출력!
	while (fgets(buffer, sizeof(buffer), f) != NULL) {
		printf("%s", buffer);
		count++;
	}

	fclose(f);
	
	if (count == 0) {
		printf(">> [안내] 장부가 비어있습니다.\n");
	}
	printf("===========================================================\n");
	printf(">> 총 %d건의 거래 내역이 조회되었습니다.\n", count);
}
