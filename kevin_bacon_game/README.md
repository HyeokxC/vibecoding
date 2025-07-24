kevin_bacon_game/
│
├── main.py                 # 메인 실행 파일
├── kb.txt                  # 입력 파일
├── requirements.txt        # 의존성 패키지 목록
│
├── config/
│   └── settings.py        # 설정 상수들
│
├── scrapers/
│   ├── __init__.py
│   ├── base_scraper.py   # 기본 스크래퍼 클래스
│   └── imdb_scraper.py   # IMDb 전용 스크래퍼
│
├── game/
│   ├── __init__.py
│   └── kevin_bacon.py    # 게임 로직
│
├── utils/
│   ├── __init__.py
│   ├── file_handler.py   # 파일 입출력
│   └── formatter.py      # 출력 포맷터
│
└── tests/                 # 테스트 파일들
    ├── __init__.py
    └── test_scraper.py
