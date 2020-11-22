# To-Do-App

[완료] TODO : 1) TEXT LCD 에서 "Task marked complete" 글귀가 2초 뜬 후 사라진다.
[완료] TODO : 2) LED - mListAdapter.getCurrentActiveItems(); 를 data로 전달하고, 해당 값을 통해 현재 active 되어있는 항목의 개수를 표현하는 함수 호출


[완료] TODO : 4-1) 7Segment - 현재 작성중인 글자 수를 보여주는 함수 호출 (data: mDescription.getText().toString().length())
[완료] TODO : 4-2) 7Segment - 현재 작성중인 글자 수를 보여주는 함수 호출 (data: 0)

//TODO : 3-4) Dot matrix - 현재 filter 이미지 보여주는 화면 종료 함수 호출

//TODO : 3-1) Dot matrix - 모든 항목 보여주는 상태라는 이미지 보여주는 함수 호출 + LCD
//TODO : 3-2) Dot matrix - Active 된 항목 보여주는 상태라는 이미지 보여주는 함수 호출 + LCD
//TODO : 3-3) Dot matrix - completed 된 항목 보여주는 상태라는 이미지 보여주는 함수 호출 + LCD





구현 함수

SSegmentWrite(int data)	data 값 출력 (범위: 0~9999)
			초기화를 한다고 하면, 이 함수를 통해 0을 전달하면 됨

DotMatrixWrite(int data)	data 값에 해당하는 이미지 출력
			data 값은 definition 파일에 따로 정의 (DOT_EMPTY, DOT_FILTER_ALL 등)

LCDWrite(int data, int delay)	data 값에 해당하는 텍스트 출력, delay만큼 sleep후 화면을 비움
			delay 값이 0이라면 sleep하지 않고, 화면은 따로 비우지 않음
			data 값은 definition 파일에 따로 정의 (LCD_FILTER_ALL, LCD_FILTER_ACTIVE 등)

LCDEmpty()		LCD 화면을 비움 

LEDWrite(int data)		data 값 출력 (범위: 0~255)






처음 켜면 LCD 리셋 (공백 8칸 + 8칸)
