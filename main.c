#include "inventory.h"

void showMenu() {
	printf("\n==========================================\n");
        printf("   [조립 PC 재고 관리 시스템 v1.0]   \n");
	printf("==========================================\n");	
	printf(" 1. Receive Item    (입고 관리)\n");
        printf(" 2. Update Item     (품목 정보 수정)\n");
        printf(" 3. Delete Item     (품목 삭제)\n");
        printf(" 4. Release Item    (출고 관리)\n");
        printf(" 5. List Inventory  (재고 현황 조회)\n");
	printf(" 6. View Sales Log  (출고 장부 조회)\n");
        printf(" 0. Save & Exit     (저장 및 종료)\n");
	printf("==========================================\n");
	printf("선택 >> ");
}

int main() {
    int choice;

    // 프로그램 시작 시 파일에서 데이터 불러오기
    loadFromFile();
    loadCustomSetsFromFile();

    while(1) {
        showMenu();

	// [예외처리] 메뉴 선택 문자 입력 차단
        if(scanf("%d", &choice) != 1) {
            printf("숫자만 입력해주세요.\n");
            clearBuffer();
            continue;
        }
        clearBuffer();

        switch(choice) {
            case 1:
                addProduct();
                break;
            case 2:
		updateProduct();
                break;
            case 3:
                deleteProduct(); 
                break;
            case 4:
		releaseProduct();
		break;
	    case 5:
                listProducts(0);
                break;
	    case 6:
		viewSalesLog();
		break;
            case 0:
                saveToFile(); // 안전하게 한 번 더 저장
		saveCustomSetsToFile();
                printf("프로그램을 안전하게 종료합니다. 수고하셨습니다!\n");
                return 0;
            default:
                printf("잘못된 선택입니다. 0~6번 사이의 번호를 입력해주세요.\n");
        }
    }
    return 0;
}
