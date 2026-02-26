#include "inventory.h"
#include "sets.h"

// 카테고리 선택 메뉴 출력
void showCategoryMenu() {
    printf("\n---------------------------- [카테고리 선택] ----------------------------\n");
    for (int i = 0; i < NUM_CATEGORIES; i++) {
        printf("%2d. %-12s", i + 1, VALID_CATEGORIES[i]);
        if ((i + 1) % 5 == 0) printf("\n");
    }
    printf("\n-------------------------------------------------------------------------\n");
}

// 제조사 선택 메뉴 출력
void showManufacturerMenu() {
    printf("\n----------------------------- [제조사 선택] -----------------------------\n");
    for (int i = 0; i < NUM_MANUFACTURERS; i++) {
        printf("%2d. %-15s", i + 1, VALID_MANUFACTURERS[i]);
        if ((i + 1) % 4 == 0) printf("\n");
    }
    printf("\n-------------------------------------------------------------------------\n");
}

// ==========================================================
// F01: 신규/기존 부품 입고 (Add) 
// ==========================================================
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
		//F01-1-0. 새 부품을 담을 메모리 준비
		Product* newNode = (Product*)malloc(sizeof(Product));

		// [예외 처리] 메모리가 부족할 때 강제 종료 방지
		if (newNode == NULL) { 
	        printf(">> [치명적 오류] 시스템 메모리가 부족합니다.\n");
	        return;
		}

                printf("\n-------- [1. 신규 부품 입고] --------\n");

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
                        	printf(" >> [오류] 1에서 %d 사이의 번호를 입력해주세요.\n", NUM_CATEGORIES);
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
                		printf(">> [오류] 잘못된 번호입니다. 다시 선택해주세요.\n");
                	}
        	}
		
		//F01-1-3. 신규 제품명 입력
		printf("제품명을 입력할 땐, 띄어쓰기 대신 '_' 를 사용해주세요.\n");
		printf("(예 : 라이젠9_9950X3D_16코어)\n");
		printf("제품명(최대 127자)>> ");
		scanf("%127[^\n]", newNode->name);
		clearBuffer();	

		//F01-1-4. 가격 및 수량 입력
		printf("매입가(원) >>");
		if (scanf("%d", &newNode->costPrice) != 1 || newNode->costPrice < 0) {
        		printf(">> [오류] 금액은 0원 이상의 숫자로 입력해주세요.\n");
        		clearBuffer(); free(newNode); return;
    		}
		clearBuffer();	

		printf("판매가(원) >>");
		if (scanf("%d", &newNode->sellPrice) != 1 || newNode->sellPrice < 0) {
        		printf(">> [오류] 금액은 0원 이상의 숫자로 입력해주세요.\n");
        		clearBuffer(); free(newNode); return;
    		}
		clearBuffer();

		printf("초기 입고 수량(개) >>");
		if (scanf("%d", &newNode->stock) != 1 || newNode->stock < 0) {
        		printf(">> [오류] 수량은 0개 이상의 숫자로 입력해주세요.\n");
        		clearBuffer(); free(newNode); return;
    		}
		clearBuffer();

		//F01-1-5. 고유 ID 부여
		newNode->id = ++lastId;
		newNode->next=NULL;

		//F01-1-6. 연결 리스트에 예쁘게 끼워 넣기 (카테고리 순서대로 정렬)
		if (head == NULL || getCategoryOrder(newNode->category) < getCategoryOrder(head->category)) {
			// 첫 번째 물건이거나, 기존 1빠보다 카테고리 순위가 앞설 때 맨 앞에 끼워넣기
			newNode->next = head;
			head = newNode;
		} else {
			// 중간이나 맨 끝에 끼워넣을 자리 찾기
			Product* current = head;
			while (current->next != NULL &&getCategoryOrder(current->next->category) 
				<=getCategoryOrder(newNode->category)) {
					current = current->next;
                	}
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
		printf("\n----------- [2. 기존 부품 입고] -----------\n");
		printf("입고할 부품의 ID를 입력하세요: ");
		
		int searchId;
		// [예외처리] ID에 문자를 치거나, 음수/0을 치는 것 방지
		if (scanf("%d", &searchId) != 1 || searchId <= 0) {
			printf(">> [오류] 올바른 ID를 숫자로 입력해주세요.\n");
			clearBuffer();
			return; // 함수 종료 (메인 메뉴로 돌아감)
		}
		clearBuffer();

		// 입력한 ID로 부품 찾기
		Product* p = searchByID(searchId);
		
		if (p != NULL) {
			// 부품을 찾았을 때
			printf("\n>> [%s] 부품을 찾았습니다. (현재 재고: %d개)\n", p->name, p->stock);
			printf("추가로 입고할 수량을 입력하세요 (취소: 0): ");
			
			int addQty;
			// [예외처리] 수량에 문자 입력 방지
			if (scanf("%d", &addQty) != 1) {
				printf(">> [오류] 수량은 숫자로만 입력해주세요.\n");
				clearBuffer();
				return; // 함수 종료
			}
			clearBuffer();

			if (addQty > 0) {
				p->stock += addQty; // 기존 재고에 더하기
				saveToFile();       // 장부에 기록 
				printf(">> 성공! %d개가 추가 입고되어 총 %d개가 되었습니다.\n", addQty, p->stock);
			} else if (addQty == 0) {
				printf(">> 입고가 취소되었습니다.\n");
			} else {
				// [예외처리] 음수 입력 방지
				printf(">> [오류] 수량은 0 이상의 숫자로 입력해주세요.\n");
			}
		} else {
			// 부품을 못 찾았을 때
			printf(">> [오류] 해당 ID(%d)를 가진 부품이 등록되어 있지 않습니다.\n", searchId);
		}
	}

	//F01-3. 신규 커스텀 세트 입고
        else if (addChoice == 3) {
		printf("\n-------------- [3. 신규 커스텀 세트 입고] -------------\n");
		saveCustomSet(); 
		
		if (customHead != NULL) {
			printf("\n방금 만든 '%s' 세트를 총 몇 대 입고하시겠습니까? (취소: 0): ", customHead->setName);
			int restockQty;
			//[예외처리] 문자 입력 방지
			if (scanf("%d", &restockQty) != 1) {
				printf(">> [오류] 입고 수량은 숫자로만 입력해주세요.\n");
				clearBuffer();
				return; // 에러 발생 시 안전하게 메인 메뉴로 복귀
			}
			clearBuffer();
			
			if (restockQty > 0) {
				// 정상 입고 처리
				processSetRestock(customHead->ids, customHead->itemCount, restockQty, customHead->setName);
			} else if (restockQty == 0) {
				// 0 입력 시 취소
				printf(">> 입고가 취소되었습니다. (세트 레시피는 정상적으로 저장되었습니다.)\n");
			} else {
				//[예외 처리] 문자, 음수 입력 방지
				printf(">> [오류] 입고 수량은 0 이상의 숫자로 입력해주세요.\n");
			}
		}
        }

	//F01-4. 기존 커스텀 세트 입고
        else if (addChoice == 4) {
		printf("\n--------------- [4. 기존 커스텀 세트 입고] ---------------\n");
		//등록된 세트가 아예 없을 때
		if (customHead == NULL) {
			printf(">> [안내] 아직 등록된 커스텀 세트가 없습니다.\n");
		} else {
			CustomSet* curr = NULL;
        		int selectNum = -1;

			// 올바른 세트의 번호를 고를 때까지 무한 반복문
			while(1) {
				CustomSet* temp = customHead;
			        int index = 1;

				// 등록된 커스텀 세트 목록 출력
				printf("\n--------------- [등록된 커스텀 세트 목록] ---------------\n");
				while ( temp != NULL ) {
					printf("%d. %s (포함 부품 %d종)\n", index++, temp->setName, temp->itemCount);
                                	temp = temp->next;
				}
				printf("0. 취소\n");
				printf("입고할 세트 번호 선택 >> ");

				// [예외처리] 입고할 세트 번호 선택시 문자 입력 방지
				if (scanf("%d",&selectNum) != 1) {
					printf(">> [오류] 숫자로만 입력해주세요!\n");
					clearBuffer();	
					continue;
				}
				clearBuffer();
				
				//0 입력 시 함수 종료 후 메인으로
				if (selectNum == 0) {
					printf(">> 입고가 취소되었습니다.\n");
					return;
				}

				if (selectNum >= 1 && selectNum < index) {
					// 입고할 세트 번호 올바른 선택 시 루프 탈출
					break;
				} else {
					printf(">> [오류] 목록에 있는 번호(1~%d)를 선택해주세요.\n", index - 1);
				}
			}

			//선택한 세트 노드 찾기
			curr = customHead;
                        for (int i = 1; i < selectNum; i++) {
				curr = curr->next;
			}

			printf("\n>> '%s' 세트를 선택하셨습니다.\n", curr->setName);

			int restockQty;
			while(1) {
				printf("입고할 수량(세트 단위) 입력(취소: 0) >> ");
					
				// [예외처리] 입고할 세트 수량 입력시 문자 입력 방지
				if (scanf("%d", &restockQty) != 1) {
					printf(">> [오류] 수량은 숫자로만 입력해주세요!\n");
                			clearBuffer();
                			continue;
				}
				clearBuffer();
				
				if (restockQty == 0) {
               				printf(">> 입고가 취소되었습니다.\n");
			                break;
			        } else if (restockQty > 0) {
					// 정상 입고 처리
					processSetRestock(curr->ids, curr->itemCount, restockQty, curr->setName);
					printf(">> 성공! [%s] 세트 %d개가 입고되었습니다.\n", curr->setName, restockQty);
					break;
				} else {
					printf(">> [오류] 수량은 0 이상의 숫자로 입력해주세요.\n");
				}
			}
		}		
	}

        //F01-5. 프리셋 세트 입고
        else if (addChoice == 5) {
                int setCatChoice;
		while (1) {
	                printf("\n--------- [5. 프리셋 세트 입고] -----------\n");
        	        printf("1. 사무/가정용 세트 (B1~B6)\n");
	                printf("2. 게이밍/그래픽 작업 세트 (G1~G6)\n");
	                printf("3. 고사양 게임/영상편집 세트 (H1~H6)\n");
	                printf("4. 프리미엄 4K 세트 (P1~P6)\n");
	                printf("5. 딥러닝/워크스테이션 세트 (W1~W6)\n");
	                printf("0. 취소\n");
	                printf("카테고리 선택 >> ");

			//[예외처리] 프리셋 세트 번호 선택시 문자 입력 방지
			if (scanf("%d", &setCatChoice) != 1) {
				printf(">> [오류] 숫자로만 입력해주세요!\n");
				clearBuffer();
				continue;
			}
			clearBuffer();

			if (setCatChoice == 0) {
				printf(">> 프리셋 입고 메뉴를 종료합니다.\n");
            			return;
			} 

			// 프리셋 세트 번호 정상 선택 시 루프 탈출
			if (setCatChoice >= 1 && setCatChoice <= 5) {
				break;
			} 
			printf(">> [오류] 잘못된 카테고리 번호입니다. (1~5번 중 선택)\n");
		}

		// 세부 세트 선택을 위한 변수들
    		int sub;
    		int setRestockQty;

                //F01-5-1. 사무/가정용 세트 입고
                if (setCatChoice == 1) {
			// [예외 처리] F01-5-1-1. 입고 세트 숫자로만 입력
			while (1) {
				printf("\n===== [사무/가정용 세트 선택] =====\n");
                        	printf("1. B1: G5905/내장 \n2. B2: 3200G/내장 \n3. B3: 13100F/GT710 \n4. B4: 8500G/내장 \n5. B5: 5600G/내장 \n6. B6: 12400/내장\n");
                        	printf("0. 취소\n선택 >> ");
				 
				if(scanf("%d", &sub) != 1) {
					printf(">> [오류] 숫자로만 입력해주세요!\n");
                			clearBuffer();
               				continue;
				}
				clearBuffer();
				
				if (sub == 0) return;
           			if (sub >= 1 && sub <= 6) break;
            			printf(">> [오류] 잘못된 세트 번호입니다. (1~6번 중 선택)\n");
			}
                        
			// [예외 처리] F01-5-1-2. 입고 수량 숫자로만 입력
			while (1) {
				printf("입고할 수량(세트 단위) 입력 (취소: 0) >> ");
				if (scanf("%d", &setRestockQty) !=1) {
					printf(">> [오류] 숫자로만 입력해주세요!\n");
					clearBuffer();
               				continue;
				}
				clearBuffer();

				if (setRestockQty == 0) {
					printf(">> 입고가 취소되었습니다.\n");
                			break;
				} else if (setRestockQty > 0) {
					 processSetRestock(officeSets[sub-1].ids,
                                                          officeSets[sub-1].size,
                                                          setRestockQty,
                                                          officeSets[sub-1].setName);
					break;
				} else {
					printf(">> [오류] 0 이상의 숫자를 입력해주세요.\n");
            			}
			}
                }

                //F01-5-2. 게이밍/그래픽 작업 세트 입고
                else if (setCatChoice == 2) {
			// [예외 처리] F01-5-2-1. 입고 세트 숫자로만 입력
			while (1) {
				printf("\n===== [게이밍/그래픽 작업 세트 선택] =====\n");
                        	printf("1. G1: 245K/내장 \n2. G2: 5600/RTX3050 \n3. G3: 5600/RX7600 \n4. G4: 12400F/RTX5050 \n5. G5: 5600/RTX5060 \n6. G6: 14400F/RTX5060\n");
                        	printf("0. 취소\n선택 >> ");
				 
				if(scanf("%d", &sub) != 1) {
					printf(">> [오류] 숫자로만 입력해주세요!\n");
                			clearBuffer();
               				continue;
				}
				clearBuffer();
				
				if (sub == 0) return;
           			if (sub >= 1 && sub <= 6) break;
            			printf(">> [오류] 잘못된 세트 번호입니다. (1~6번 중 선택)\n");
			}
                        
			// [예외 처리] F01-5-2-2. 입고 수량 숫자로만 입력
			while (1) {
				printf("입고할 수량(세트 단위) 입력 (취소: 0) >> ");
				if (scanf("%d", &setRestockQty) !=1) {
					printf(">> [오류] 숫자로만 입력해주세요!\n");
					clearBuffer();
               				continue;
				}
				clearBuffer();

				if (setRestockQty == 0) {
					printf(">> 입고가 취소되었습니다.\n");
                			break;
				} else if (setRestockQty > 0) {
					 processSetRestock(gamingSets[sub-1].ids,
                                                          gamingSets[sub-1].size,
                                                          setRestockQty,
                                                          gamingSets[sub-1].setName);
					break;
				} else {
					printf(">> [오류] 0 이상의 숫자를 입력해주세요.\n");
            			}
			}
                }

		//F01-5-3. 고사양 게임/영상편집 세트 입고
                else if (setCatChoice == 3) {
			// [예외 처리] F01-5-3-1. 입고 세트 숫자로만 입력
			while (1) {
				printf("\n===== [고사양 게임/영상편집 세트 선택] =====\n");
                        	printf("1. H1: 7400F/RTX5060 \n2. H2: 7400F/RX9060 XT \n3. H3: 9600/RX9060 XT \n4. H4: 7400/RTX5060 Ti \n5. H5: 14600KF/RTX5060 \n6. H6: 9600/RTX5060 Ti\n");
                        	printf("0. 취소\n선택 >> ");
				 
				if(scanf("%d", &sub) != 1) {
					printf(">> [오류] 숫자로만 입력해주세요!\n");
                			clearBuffer();
               				continue;
				}
				clearBuffer();
				
				if (sub == 0) return;
           			if (sub >= 1 && sub <= 6) break;
            			printf(">> [오류] 잘못된 세트 번호입니다. (1~6번 중 선택)\n");
			}
                        
			// [예외 처리] F01-5-3-2. 입고 수량 숫자로만 입력
			while (1) {
				printf("입고할 수량(세트 단위) 입력 (취소: 0) >> ");
				if (scanf("%d", &setRestockQty) !=1) {
					printf(">> [오류] 숫자로만 입력해주세요!\n");
					clearBuffer();
               				continue;
				}
				clearBuffer();

				if (setRestockQty == 0) {
					printf(">> 입고가 취소되었습니다.\n");
                			break;
				} else if (setRestockQty > 0) {
					 processSetRestock(proSets[sub-1].ids,
                                                          proSets[sub-1].size,
                                                          setRestockQty,
                                                          proSets[sub-1].setName);
					break;
				} else {
					printf(">> [오류] 0 이상의 숫자를 입력해주세요.\n");
            			}
			}
                }
		
		//F01-5-4. 프리미엄 4K 세트 입고
                else if (setCatChoice == 4) {
			// [예외 처리] F01-5-4-1. 입고 세트 숫자로만 입력
			while (1) {
				printf("\n===== [프리미엄 4K 세트 선택] =====\n");
                        	printf("1. P1: 14600KF/RTX5070 \n2. P2: 9600/RTX5070 \n3. P3: 265KF/RTX5070 \n4. P4: 7800X3D/RX9070 XT \n5. P5: 7800X3D/RTX5070 Ti \n6. P6: 9800X3D/RTX5070 Ti\n");
                        	printf("0. 취소\n선택 >> ");
				 
				if(scanf("%d", &sub) != 1) {
					printf(">> [오류] 숫자로만 입력해주세요!\n");
                			clearBuffer();
               				continue;
				}
				clearBuffer();
				
				if (sub == 0) return;
           			if (sub >= 1 && sub <= 6) break;
            			printf(">> [오류] 잘못된 세트 번호입니다. (1~6번 중 선택)\n");
			}
                        
			// [예외 처리] F01-5-4-2. 입고 수량 숫자로만 입력
			while (1) {
				printf("입고할 수량(세트 단위) 입력 (취소: 0) >> ");
				if (scanf("%d", &setRestockQty) !=1) {
					printf(">> [오류] 숫자로만 입력해주세요!\n");
					clearBuffer();
               				continue;
				}
				clearBuffer();

				if (setRestockQty == 0) {
					printf(">> 입고가 취소되었습니다.\n");
                			break;
				} else if (setRestockQty > 0) {
					 processSetRestock(ultraSets[sub-1].ids,
                                                          ultraSets[sub-1].size,
                                                          setRestockQty,
                                                          ultraSets[sub-1].setName);
					break;
				} else {
					printf(">> [오류] 0 이상의 숫자를 입력해주세요.\n");
            			}
			}
                }
		
		//F01-5-5. 딥러닝/워크스테이션 세트 입고
                else if (setCatChoice == 5) {
			// [예외 처리] F01-5-5-1. 입고 세트 숫자로만 입력
			while (1) {
				printf("\n===== [딥러닝/워크스테이션 세트 입고] =====\n");
                        	printf("1. W1: 7800X3D/RTX5080 \n2. W2: 265KF/RTX5080 \n3. W3: 9800X3D/RTX5080 \n4. W4: 14900KF/RTX5080 \n5. W5: 9900XRTX5090 \n6. W6: 285K/RTX5090\n");
                        	printf("0. 취소\n선택 >> ");
				 
				if(scanf("%d", &sub) != 1) {
					printf(">> [오류] 숫자로만 입력해주세요!\n");
                			clearBuffer();
               				continue;
				}
				clearBuffer();
				
				if (sub == 0) return;
           			if (sub >= 1 && sub <= 6) break;
            			printf(">> [오류] 잘못된 세트 번호입니다. (1~6번 중 선택)\n");
			}
                        
			// [예외 처리] F01-5-5-2. 입고 수량 숫자로만 입력
			while (1) {
				printf("입고할 수량(세트 단위) 입력 (취소: 0) >> ");
				if (scanf("%d", &setRestockQty) !=1) {
					printf(">> [오류] 숫자로만 입력해주세요!\n");
					clearBuffer();
               				continue;
				}
				clearBuffer();

				if (setRestockQty == 0) {
					printf(">> 입고가 취소되었습니다.\n");
                			break;
				} else if (setRestockQty > 0) {
					 processSetRestock(workSets[sub-1].ids,
                                                          workSets[sub-1].size,
                                                          setRestockQty,
                                                          workSets[sub-1].setName);
					break;
				} else {
					printf(">> [오류] 0 이상의 숫자를 입력해주세요.\n");
            			}
			}
                }

		else {
                printf(">> [오류] 잘못된 입력입니다.\n");
        	}
	}
}
