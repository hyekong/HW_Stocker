#include "inventory.h"
#include "sets.h"

// ==========================================================
// [2] 시스템 및 유틸리티 함수 (System & Utils)
//      ㄴ입출고 실무 로직
// ==========================================================

// 세트 입고
void processSetRestock(int ids[], int size, int qty, const char* setName) {
	// 비정상 수량이 넘어오는 것 차단
	if (qty <= 0) return;

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
	
	saveToFile(); // 재고 수정 후 파일에 저장!
	printf(">> 입고 처리가 완료되었습니다.\n");
}

// 세트 출고 
void processSetRelease(int ids[], int size,int qty, const char* setName) {
	// 비정상 수량이 넘어오는 것 차단
	if (qty <= 0) return;

	printf("\n>> [%s] 세트 출고 처리를 시작합니다.\n", setName);

	// 1단계에서 찾은 부품 주소들을 기억해둘 임시 배열 생성
	// (C99 표준부터 지원되는 가변 길이 배열(VLA)을 활용)
	Product* foundProducts[size]; 
	int isEnough = 1;

	// 세트 출고 1단계: 모든 부품의 재고가 충분한지 확인+ 찾은 부품 저장 
	for (int i = 0; i < size ; i++) {
		foundProducts[i] = searchByID(ids[i]); // 배열 저장
		Product* p = foundProducts[i];         // 저장 후 바로 사용

		if (p == NULL) {
			printf(" - [경고] ID %d 부품을 찾을 수 없습니다.\n", ids[i]);
			isEnough = 0;
		} else if (p->stock < qty) {
			printf(" - [재고 부족] %s (현재 재고: %d개, 필요 수량: %d개)\n", p->name, p->stock, qty);
			isEnough = 0;
		}
	}
		
	// 세트 출고 2단계: 재고가 모두 충분할 때만 실제 출고 진행
	if (isEnough) {
		for (int i=0; i<size; i++) {
			Product* p = foundProducts[i]; //저장 주소 사용
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

// 매출 조회
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
// F06. 매출 장부 기록 및 조회 (Sales & Logs)
// ==========================================================
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

// ==========================================================
// F04. 출고 처리 핵심 로직 (Release Logic)
// ==========================================================
void releaseProduct() {
	printf("\n------- [출고 관리] -------\n");
   	printf("1. 단품 출고 \n");
	printf("2. 커스텀 세트 출고 \n");
	printf("3. 프리셋 세트 출고 \n");
	printf("0. 뒤로가기\n");
	printf("============================\n");

	int relChoice;
	while(1) {
		printf("선택 >> ");
		if (scanf("%d", &relChoice) != 1) {
			printf(">> [오류] 숫자로 입력해주세요.\n");
			clearBuffer();
			continue;
		}
		clearBuffer();
		if (relChoice >= 0 && relChoice <= 3) break;
		printf(">> [오류] 0~3 사이의 번호를 선택해주세요.\n");
	}
	//F04-0. 취소 
	if (relChoice == 0) return;

	//F04-1. 단품 출고
	if (relChoice == 1) {
		listProducts(0);
		printf("\n------- [단품 출고] -------\n");
	
		Product* target = NULL;
		int targetNo;

		//[예외처리] 출고 번호 선택 예외 처리
		while(1) {
			printf("\n\n출고할 품목의 번호(No.)를 입력하세요(취소: 0) >> ");
			if (scanf("%d", &targetNo) != 1 ) {
                        	printf(">> [오류] 숫자로만 입력해주세요.\n");
                        	clearBuffer();
	                        continue;
			}
			clearBuffer();

			if (targetNo == 0) {
				printf(">> 단품 출고가 취소되었습니다.\n");
				return;
			}

			target = searchByNo(targetNo);
			
			if (target == NULL) {
				printf(">> [오류] 해당 번호(No. %d)의 상품을 찾을 수 없습니다. 목록을 다시 확인해주세요.\n", targetNo);
				continue;
			}
			break;
                }
			
		printf(">> 선택된 상품: [%s] %s  %s (재고: %d개)\n", 
               		target->category,target->manufacturer, target->name, target->stock);

		//[예외처리] 출고 수량 예외 처리
		int relQty = 0;
		while(1) {
			printf("출고 수량을 입력하세요: (취소: 0) >>");
			if (scanf("%d",&relQty) != 1 ) {
                        	printf(">> [오류] 숫자로만 입력해주세요.\n");
                        	clearBuffer();
                        	continue; 
			}
			clearBuffer();

			if (relQty == 0) {
		                printf(">> 단품 출고가 취소되었습니다.\n");
		                return;
       			}
			
			if (relQty < 0) {
	                	printf(">> [오류] 1개 이상의 올바른 수량을 입력하세요.\n");
		        } else if (relQty > target->stock) {
			        printf(">> [거부] 재고가 부족합니다! (현재 재고: %d개)\n", target->stock);
	  	        } else {
		                // 정상 출고 처리
		                target->stock -= relQty;
		                logSale(target->name, relQty, target->costPrice, target->sellPrice);
		                saveToFile();
		                printf(">> [성공!] %d개 출고 완료! (남은 재고: %d개)\n", relQty, target->stock);
		                break; // 출고 완료 후 루프 탈출
           		 }
		}
	}

	//F04-2. 커스텀 세트 출고
	else if (relChoice == 2) {
		printf("\n------- [ 커스텀 세트 출고 ] -------\n");
		if (customHead == NULL) {
			printf(">> [안내] 등록된 커스텀 세트가 없습니다.\n");
		} else {
			CustomSet* curr = customHead;
			int selectNum;

			//[예외처리] 커스텀 세트 번호 선택 시, 예외 처리
			while (1) {
				CustomSet* temp = customHead;
		                int index = 1;

				printf("\n--------------- [등록된 커스텀 세트 목록] ---------------\n");
                		while (temp != NULL) {
					printf("%d. %s (포함 부품 %d종)\n", index++, temp->setName, temp->itemCount);
					temp = temp->next;
				}
				printf("0. 취소\n출고할 세트 번호 선택 >> ");

				// [예외처리] 문자 입력 방지
				if (scanf("%d", &selectNum) != 1) {
					printf(">> [오류] 숫자로만 입력해주세요!\n");
					clearBuffer();
					continue;
				}
				clearBuffer();

				if (selectNum == 0) {
					printf(">> 출고가 취소되었습니다.\n");
					return; // 메인 메뉴로 탈출
				}

				if (selectNum > 0 && selectNum < index) {
					break; // 올바른 번호 선택 시 루프 탈출
				} else {
					printf(">> [오류] 목록에 있는 번호(1~%d)를 선택해주세요.\n", index - 1);
				}
			}
			// 선택한 세트 노드 찾기
		        curr = customHead;
			for (int i = 1; i < selectNum; i++) {
                		curr = curr->next;
            		}

		        printf("\n>> '%s' 세트를 선택하셨습니다.\n", curr->setName);


			//[예외처리] 커스텀 세트 출고 수량 예외 처리
			int relQty;
			while(1) {
				printf("출고할 세트 번호 선택 (취소: 0) >> ");
				
				// [예외처리] 수량 문자 입력 방지
				if (scanf("%d", &relQty) != 1) {
					printf(">> [오류] 수량은 숫자로만 입력해주세요!\n");
					clearBuffer();
					continue;
				}
				clearBuffer();
			
				if (relQty == 0) {
					printf(">> 출고가 취소되었습니다.\n");
					break; // 수량 입력 취소 시 루프 탈출
				} else if (relQty > 0) {
					// 정상 출고 처리 (processSetRelease 함수가 재고 부족 여부를 알아서 판단)
					processSetRelease(curr->ids, curr->itemCount, relQty, curr->setName);
					break;
				} else { 
					printf(">> [오류] 수량은 1 이상의 숫자로 입력해주세요.\n");
				}
			}
		}
	}

	//F04-3. 프리셋 세트 출고
	else if (relChoice == 3) {
		int setChoice;
	
		while(1) {
			printf("\n[세트 카테고리 선택]\n");
        		printf("1. 사무/가정용 (B1~B6)\n");
        		printf("2. 게이밍/그래픽 작업 (G1~G6)\n");
        		printf("3. 고사양 게임/영상편집 (H1~H6)\n");
			printf("4. 프리미엄 4K (P1~P6)\n");
			printf("5. 딥러닝/워크스테이션 (W1~W6)\n");
	        	printf("0. 취소\n");
			printf("======================================\n");
       		 	printf("선택 >> ");

			if(scanf("%d", &setChoice) != 1) {
	                	printf(">> [오류] 숫자로만 입력해주세요.\n");
		                clearBuffer();
		                continue;
	                }
			clearBuffer();

			if (setChoice == 0) {
	       	         	printf(">> 프리셋 세트 출고가 취소되었습니다.\n");
	                	return; 
	            	}
			
			if (setChoice >= 1 && setChoice <= 5) break;
			printf(">> [오류] 1~5번 사이의 메뉴를 선택해주세요.\n");
		}
		
		int sub;
		int relQty;

		//F04-3-1. 사무/가정용 (B1~B6)
		if (setChoice == 1) {
			while(1) {
				printf("\n--- [사무용 세트 목록] ---\n");
	        		for (int i = 0; i < OFFICE_SETS_COUNT; i++) {
	            			printf("%d. %s\n", i + 1, officeSets[i].setName);
	       			}
		      		printf("0. 취소\n선택 >> ");
        
				//F04-3-1-1. 사무/가정용 세트 선택 예외 처리
				if (scanf("%d", &sub) != 1) {	
					printf(">> [오류] 숫자로만 입력해주세요.\n");
					clearBuffer();
					continue;
				}
				clearBuffer();
        
				if(sub == 0) return;
       		 		if (sub > 0 && sub <= OFFICE_SETS_COUNT) break; // 정상 통과
				printf(">> [오류] 올바른 번호를 선택해주세요.\n");
			}
	
			//F04-3-1-2. 사무/가정용 출고 수량 예외 처리
			while(1) {
				printf("출고할 '%s' 세트의 수량을 입력하세요: ", officeSets[sub-1].setName);
		                if (scanf("%d", &relQty) != 1) {
					printf(">> [오류] 숫자로만 입력해주세요.\n");
					clearBuffer();	
					continue;
				}
                		clearBuffer();

				if (relQty == 0) return;
				if (relQty > 0) {
	            			processSetRelease(officeSets[sub-1].ids, 
	                                officeSets[sub-1].size, 
					relQty,
	                                officeSets[sub-1].setName);
					break;
				}
				printf(">> [오류] 1 이상의 올바른 수량을 입력해주세요.\n");
        		}
		}

                //F04-3-2. 게이밍/그래픽 작업 (G1~G6)
                else if (setChoice == 2) {
                        while(1) {
                                printf("\n--- [게이밍/그래픽 작업 세트 목록] ---\n");
                                for (int i = 0; i < GAMING_SETS_COUNT; i++) {
                                        printf("%d. %s\n", i + 1, gamingSets[i].setName);
                                }
                                printf("0. 취소\n선택 >> ");

                                //F04-3-2-1. 게이밍/그래픽 세트 종류 선택 예외 처리
                                if (scanf("%d", &sub) != 1) {
                                        printf(">> [오류] 숫자로만 입력해주세요.\n");
                                        clearBuffer();
                                        continue;
                                }
                                clearBuffer();
                                if(sub == 0) return;
                                if (sub > 0 && sub <= GAMING_SETS_COUNT) break; // 정상 통과
                                printf(">> [오류] 올바른 번호를 선택해주세요.\n");
                        }

                        //F04-3-2-2. 게이밍/그래픽 세트 출고 수량 예외 처리
                        while(1) {
                                printf("출고할 '%s' 세트의 수량을 입력하세요: ", gamingSets[sub-1].setName);
                                if (scanf("%d", &relQty) != 1) {
                                        printf(">> [오류] 숫자로만 입력해주세요.\n");
                                        clearBuffer();
                                        continue;
                                }
                                clearBuffer();

                                if (relQty == 0) return;
                                if (relQty > 0) {
                                        processSetRelease(gamingSets[sub-1].ids,
                                        gamingSets[sub-1].size,
                                        relQty,
                                        gamingSets[sub-1].setName);
                                        break;
                                }
                                printf(">> [오류] 1 이상의 올바른 수량을 입력해주세요.\n");
                        }
                }

                //F04-3-3. 고사양 게임/영상편집 (H1~H6)
                else if (setChoice == 3) {
                        while(1) {
                                printf("\n--- [고사양 게임/영상편집 세트 목록] ---\n");
                                for (int i = 0; i < PRO_SETS_COUNT; i++) {
                                        printf("%d. %s\n", i + 1, proSets[i].setName);
                                }
                                printf("0. 취소\n선택 >> ");

                                //F04-3-3-1. 고사양 게임/영상편집 세트 종류 선택 예외 처리
                                if (scanf("%d", &sub) != 1) {
                                        printf(">> [오류] 숫자로만 입력해주세요.\n");
                                        clearBuffer();
                                        continue;
                                }
                                clearBuffer();
                                if(sub == 0) return;
                                if (sub > 0 && sub <= PRO_SETS_COUNT) break; // 정상 통과
                                printf(">> [오류] 올바른 번호를 선택해주세요.\n");
                        }

                        //F04-3-3-2. 고사양 게임/영상편집 세트 출고 수량 예외 처리
                        while(1) {
                                printf("출고할 '%s' 세트의 수량을 입력하세요: ", proSets[sub-1].setName);
                                if (scanf("%d", &relQty) != 1) {
                                        printf(">> [오류] 숫자로만 입력해주세요.\n");
                                        clearBuffer();
                                        continue;
                                }
                                clearBuffer();

                                if (relQty == 0) return;
                                if (relQty > 0) {
                                        processSetRelease(proSets[sub-1].ids,
                                        proSets[sub-1].size,
                                        relQty,
                                        proSets[sub-1].setName);
                                        break;
                                }
                                printf(">> [오류] 1 이상의 올바른 수량을 입력해주세요.\n");
                        }
                }

                //F04-3-4. 프리미엄 4K (P1~P6)
                else if (setChoice == 4) {
                        while(1) {
                                printf("\n--- [프리미엄 4K 세트 목록] ---\n");
                                for (int i = 0; i < ULTRA_SETS_COUNT; i++) {
                                        printf("%d. %s\n", i + 1, ultraSets[i].setName);
                                }
                                printf("0. 취소\n선택 >> ");

                                //F04-3-4-1. 프리미엄 4K 세트 종류 선택 예외 처리
                                if (scanf("%d", &sub) != 1) {
                                        printf(">> [오류] 숫자로만 입력해주세요.\n");
                                        clearBuffer();
                                        continue;
                                }
                                clearBuffer();
                                if(sub == 0) return;
                                if (sub > 0 && sub <= ULTRA_SETS_COUNT) break; // 정상 통과
                                printf(">> [오류] 올바른 번호를 선택해주세요.\n");
                        }

                        //F04-3-4-2. 프리미엄 4K 세트 출고 수량 예외 처리
                        while(1) {
                                printf("출고할 '%s' 세트의 수량을 입력하세요: ", ultraSets[sub-1].setName);
                                if (scanf("%d", &relQty) != 1) {
                                        printf(">> [오류] 숫자로만 입력해주세요.\n");
                                        clearBuffer();
                                        continue;
                                }
                                clearBuffer();

                                if (relQty == 0) return;
                                if (relQty > 0) {
                                        processSetRelease(ultraSets[sub-1].ids,
                                        ultraSets[sub-1].size,
                                        relQty,
                                        ultraSets[sub-1].setName);
                                        break;
                                }
                                printf(">> [오류] 1 이상의 올바른 수량을 입력해주세요.\n");
                        }
                }

                //F04-3-5. 딥러닝/워크스테이션 (W1~W6)
                else if (setChoice == 5) {
                        while(1) {
                                printf("\n--- [딥러닝/워크스테이션] ---\n");
                                for (int i = 0; i < WORK_SETS_COUNT; i++) {
                                        printf("%d. %s\n", i + 1, workSets[i].setName);
                                }
                                printf("0. 취소\n선택 >> ");

                                //F04-3-5-1. 딥러닝/워크스테이션 세트 종류 선택 예외 처리
                                if (scanf("%d", &sub) != 1) {
                                        printf(">> [오류] 숫자로만 입력해주세요.\n");
                                        clearBuffer();
                                        continue;
                                }
                                clearBuffer();
                                if(sub == 0) return;
                                if (sub > 0 && sub <= WORK_SETS_COUNT) break; // 정상 통과
                                printf(">> [오류] 올바른 번호를 선택해주세요.\n");
                        }

                        //F04-3-5-2. 딥러닝/워크스테이션 세트 출고 수량 예외 처리
                        while(1) {
                                printf("출고할 '%s' 세트의 수량을 입력하세요: ", workSets[sub-1].setName);
                                if (scanf("%d", &relQty) != 1) {
                                        printf(">> [오류] 숫자로만 입력해주세요.\n");
                                        clearBuffer();
                                        continue;
                                }
                                clearBuffer();

                                if (relQty == 0) return;
                                if (relQty > 0) {
                                        processSetRelease(workSets[sub-1].ids,
                                        workSets[sub-1].size,
                                        relQty,
                                        workSets[sub-1].setName);
                                        break;
                                }
                                printf(">> [오류] 1 이상의 올바른 수량을 입력해주세요.\n");
                        }
                }

	}
	else {
		printf(">> [오류] 잘못된 메뉴 선택입니다.\n");
		clearBuffer();
	}
}

