#include "inventory.h"

void showMenu() {
    printf("\n=== [조립 PC 재고 관리 시스템] ===\n");
    printf("1. 신규 품목 등록 (Add)\n");
    printf("2. 품목 정보 수정 (Update)\n");
    printf("3. 출고 관리 (Release) - 내일 구현\n");
    printf("4. 재고 현황 조회 (List)\n");
    printf("0. 저장 및 종료 (Exit)\n");
    printf("================================\n");
    printf("선택 >> ");
}

int main() {
    int choice;

    // 프로그램 시작 시 파일에서 데이터 불러오기
    loadFromFile();

    while(1) {
        showMenu();
        if(scanf("%d", &choice) != 1) {
            printf("숫자만 입력해주세요.\n");
            clearBuffer();
            continue;
        }
        clearBuffer();

        switch(choice) {
            case 1:
                addProduct();
                saveToFile(); // 추가 후 즉시 저장
                break;
            case 2:
		updateProduct();
                saveToFile(); // 수정 후 변경된 데이터 즉시 덮어쓰기
                break;
            case 3:
                printf(">> 출고 기능은 내일 구현 예정입니다.\n");
                break;
            case 4:
                listProducts();
                break;
            case 0:
                saveToFile(); // 안전하게 한 번 더 저장
                printf("프로그램을 종료합니다.\n");
                return 0;
            default:
                printf("잘못된 선택입니다. 다시 입력해주세요.\n");
        }
    }
    return 0;
}
