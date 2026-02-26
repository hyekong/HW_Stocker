#include "inventory.h"

// ==========================================================
// [2] 시스템 및 유틸리티 함수 (System & Utils)
//      ㄴ커스텀 세트 관리
// ==========================================================

// [2-1] 커스텀 세트 추가
void saveCustomSet() {
	CustomSet* newNode = (CustomSet*)malloc(sizeof(CustomSet));
	if (newNode == NULL) {
        	printf(">> [오류] 시스템 메모리가 부족합니다.\n");
	        return;
	}

	printf("\n--- [새 커스텀 세트 만들기] ---\n");
	printf("세트 이름 (공백 없이 입력, 최대 49자) >> ");

	//[예외처리] 긴 이름을 입력해서 메모리 터지는것(버퍼 오버플로우) 방지
	scanf("%49s", newNode->setName);
	clearBuffer();

	int count = 0;
	int inputId;

	printf("\n포함할 부품의 ID를 하나씩 입력하세요. (종료: 0)\n");

	while (count < 20) {
		while(1) {
			printf("[%d번째 부품] ID 입력 >> ", count + 1);
			if (scanf("%d", &inputId) != 1) {
                		printf(">> [오류] 숫자로만 입력해주세요!\n");
                		clearBuffer();
                		continue; // 다시 입력받기
            		}
	                clearBuffer();
			break; // 올바른 숫자 입력 시 루프 탈출
		}

                if (inputId == 0) break; // 0 누르면 입력 종료

                if (searchByID(inputId) != NULL) {
                        newNode->ids[count] = inputId;
                        count++;
                        printf(">> ID %d번 부품이 담겼습니다.\n", inputId);
                } else {
                        printf(">> [경고] 존재하지 않는 부품입니다.\n");
                }
	}

	// 담은 부품이 하나라도 있을 때만 등록
	if (count > 0) {
		newNode->itemCount = count;
		newNode->next = customHead;
		customHead = newNode;

		saveCustomSetsToFile(); 
		printf("\n>> 성공! '%s' 세트(부품 %d종) 등록 완료!\n", newNode->setName, count);
	} else {
		printf("\n>> 담은 부품이 없어 세트 만들기가 취소되었습니다.\n");
		free(newNode);
	}
}
// [2-2] 커스텀 세트 삭제
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

// [2-3] 커스텀 세트 파일에 저장
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

// [2-4] 커스텀 세트 파일 불러오기
void loadCustomSetsFromFile() {
	FILE* f = fopen("custom_sets.txt", "r");
	if (f == NULL) return; // 파일 없으면 그냥 넘어감

	while (1) {
		CustomSet* newNode = (CustomSet*)malloc(sizeof(CustomSet));
		if (newNode == NULL) break;
	// [예외처리] 누군가 텍스트 파일을 잘못 건드려서 형식이 깨졌을 때의 무한 루프 차단

		int res = fscanf(f, "%49s %d", newNode->setName, &newNode->itemCount);
        
	        if (res == EOF) {
       		     free(newNode);
       		     break; // 파일 끝이면 종료
       		}
		if (res != 2) {
            		// 매칭 실패 시 무한 루프 방지를 위해 한 줄을 통째로 읽어서 버림
	       		free(newNode);
            		char junk[256];
            		fgets(junk, sizeof(junk), f); 
            		continue; 
        	}

		for (int i = 0; i < newNode->itemCount; i++) {
			fscanf(f, "%d", &newNode->ids[i]);
		}

		newNode->next = customHead;
		customHead = newNode;
	}
	fclose(f);
}


// ==========================================================
//  F02: 품목 정보 수정 (Update)
// ==========================================================
void updateProduct() {
	if (head == NULL) {
        	printf("\n>> 수정할 재고가 없습니다.\n");
       		return;
	}

	listProducts(0);

	int targetNo;
	Product* target = NULL;

	//[예외처리] 올바른 No(번호) 선택 시 루프 탈출
	while(1) {
		printf("\n-------- 품목 정보 수정 --------\n");
		printf("\n수정할 부품의 번호(No.)를 입력하세요: ");
		if (scanf("%d", &targetNo) != 1) {
        		printf("X 숫자로만 입력해주세요!\n");
        		clearBuffer();
			continue;
    		}
    		clearBuffer();
		if (targetNo == 0) {
            		printf(">> 수정을 취소합니다.\n");
            		return;
        	}

        	target = searchByNo(targetNo);

		if (target == NULL) {
	            	printf(">> [오류] 입력하신 번호(No. %d)에 해당하는 부품이 없습니다.\n", targetNo);
 	           	continue; // 다시 번호 입력받기
        	}
        	break; // 부품 찾기 성공!
	}

	// 3. 찾았다면 그 위치의 데이터를 즉시 수정! (ID는 건드리지 않음)
	int upChoice;
	while(1) {
		printf("\n>> [%s] %s 제품을 수정합니다.\n", target->manufacturer, target->name);
		printf("1. 매입가(원가) 수정 (현재: %d원)\n", target->costPrice);
		printf("2. 판매가(출고가) 수정 (현재: %d원)\n", target->sellPrice);
		printf("3. 재고 수량 수정 (현재: %d개)\n", target->stock);
		printf("0. 수정 취소\n");
		printf("선택 >> ");

		if (scanf("%d", &upChoice) != 1) {
            		printf(">> [오류] 숫자로만 입력해주세요!\n");
            		clearBuffer();
            		continue;
        	}
        	clearBuffer();
	
        	if (upChoice >= 0 && upChoice <= 3) break;
        	printf(">> [오류] 0~3 사이의 메뉴를 선택해주세요.\n");
	}

	if (upChoice == 0) {
        	printf(">> 수정을 취소합니다.\n");
        	return; // 값 변경이 없으므로 저장하지 않고 종료
    	}

	// [예외처리] 음수, 문자 방지
	int newValue;
	while(1) {
        	if (upChoice == 1) printf("새 매입가 입력 >> ");
        	else if (upChoice == 2) printf("새 판매가 입력 >> ");
        	else if (upChoice == 3) printf("새 재고 수량 입력 >> ");

        	if (scanf("%d", &newValue) != 1) {
            		printf(">> [오류] 숫자로만 입력해주세요!\n");
            		clearBuffer();
            		continue;
        	}
        	clearBuffer();

        	if (newValue < 0) {
            		printf(">> [오류] 0 이상의 올바른 숫자를 입력하세요.\n");
            		continue;
        	}
        
        	break; // 올바른 값 입력 완료
	}

	// 값 적용
    	if (upChoice == 1) {
        	target->costPrice = newValue;
        	printf(">> 매입가가 %d원으로 수정되었습니다.\n", newValue);
    	} else if (upChoice == 2) {
        	target->sellPrice = newValue;
        	printf(">> 판매가가 %d원으로 수정되었습니다.\n", newValue);
    	} else if (upChoice == 3) {
        	target->stock = newValue;
        	printf(">> 재고 수량이 %d개로 수정되었습니다.\n", newValue);
    	}

    	saveToFile(); //실제 데이터가 바뀌었을 때만 장부(파일)에 저장
}

// ==========================================================
// F03: 품목 및 커스텀 세트 삭제 (Delete)
// ==========================================================
void deleteProduct() {
	//[예외처리] 올바른 No(번호) 선택 시 루프 탈출
    	while (1) {
	        printf("\n===== [ 품목 삭제 관리 ] =====\n");
        	printf("1. 단일 품목 삭제\n");
        	printf("2. 커스텀 세트 삭제\n");
        	printf("0. 뒤로가기\n");
        	printf("선택 >> ");

        	int delChoice;
        	if (scanf("%d", &delChoice) != 1) {
        	    printf(">> [오류] 숫자로만 입력해주세요!\n");
        	    clearBuffer();
        	    continue;
        	}
	        clearBuffer();

        	if (delChoice == 0) return; // 메인으로 돌아가기

		// F03-1. 단일 품목 삭제
        	if (delChoice == 1) {
            		if (head == NULL) {
                		printf("\n>> [안내] 삭제할 단일 품목이 없습니다.\n");
                		continue; 
            		}
            	listProducts(0); // 목록 출력

	        int targetNo;
	        while(1) {
                	printf("\n------ [ 1. 단일 품목 삭제 ] ------\n");
                	printf("삭제할 항목의 번호(No.)를 입력하세요 (취소: 0) >> ");
                
                	if (scanf("%d", &targetNo) != 1) {
                    		printf(">> [오류] 숫자로만 입력해주세요!\n");
                    		clearBuffer();
                    		continue;
                	}
                	clearBuffer();

                	if (targetNo == 0) {
                   		printf(">> 단일 품목 삭제를 취소합니다.\n");
                    		break; // 다시 삭제 메뉴로
                	}

                	// 삭제를 위한 탐색 (앞칸 prev 와 현재칸 curr 찾기)
                	Product *curr = head, *prev = NULL;
                	int count = 1;

	                while (curr != NULL && count < targetNo) {
                    		prev = curr;
                    		curr = curr->next;
                    	count++;
                	}

                	if (curr == NULL || targetNo < 1) {
                    		printf(">> [오류] 해당 번호의 항목을 찾을 수 없습니다.\n");
                    		continue; 
                	}

                	// 최종 확인 (안전장치)
                	char confirm;
                	printf(">> [%s] %s 제품을 정말 삭제하시겠습니까? (y/n) >> ", curr->manufacturer, curr->name);
                	scanf(" %c", &confirm); 
                	clearBuffer();

	                if (confirm == 'y' || confirm == 'Y') {
	                	if (prev == NULL) { head = curr->next; }
                    		else { prev->next = curr->next; }
                    
                    	free(curr); // 메모리 비우기
                    	printf(">> 삭제 완료!\n");
                    	saveToFile(); // 변경사항 저장
                	} else {
                		printf(">> 삭제가 취소되었습니다.\n");
                	}
                	break; // 삭제가 끝났든 취소했든 루프 탈출
            	}
        } 
        
	// F03-2. 커스텀 세트 삭제
        else if (delChoice == 2) {
		if (customHead == NULL) {
                	printf("\n>> [안내] 등록된 커스텀 세트가 없습니다.\n");
                	continue;
            	}

		while(1) {
                	printf("\n----- [ 2. 커스텀 세트 삭제 ] -----\n");
                	CustomSet* curr = customHead;
                	int index = 1;
                	while (curr != NULL) {
                    		printf("%d. %s (포함 부품 %d종)\n", index++, curr->setName, curr->itemCount);
                    		curr = curr->next;
                	}

	                printf("0. 취소\n삭제할 세트 번호 선택 >> ");
	                int delSetChoice;
	                if (scanf("%d", &delSetChoice) != 1) {
	                    	printf(">> [오류] 숫자로만 입력해주세요!\n");
                    		clearBuffer();
                    	continue;
                	}
	                clearBuffer();

                	if (delSetChoice == 0) {
                    		printf(">> 삭제가 취소되었습니다.\n");
                    		break;
                	}

	                if (delSetChoice > 0 && delSetChoice < index) {
                    		curr = customHead;
                    		CustomSet* prev = NULL;

                    	// 세트 삭제를 위한 탐색
                    	for (int i = 1; i < delSetChoice; i++) {
                        	prev = curr;
                        	curr = curr->next;
                    	}

                    	if (prev == NULL) {
                        	customHead = curr->next;
                    	} else {
                        	prev->next = curr->next;
                    	}	

                    	printf(">> '%s' 커스텀 세트가 삭제되었습니다.\n", curr->setName);
                   	free(curr);
                    	saveCustomSetsToFile();
                    	break;
                } else {
                    	printf(">> [오류] 목록에 있는 번호를 선택해주세요.\n");
                }
	}
        } else {
            	printf(">> [오류] 0~2 사이의 메뉴를 선택해주세요.\n");
        	}
	}
}

// ==========================================================
// F05: 재고 현황 조회 (List)
// =========================================================
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

