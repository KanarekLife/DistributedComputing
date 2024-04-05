#set page(
  paper: "a4",
  margin: 1.5cm,
  numbering: "1 / 1",
  footer: context [
    #set text(8pt)
    *Tanks* - dokumentacja
    #h(1fr)
    #counter(page).display(
      "1 / 1",
      both: true
    )
  ]
)

#set document(
  title: "Tanks - dokumentacja",
  author: "Bartłomiej Krawisz, Jakub Bronowski, Stanisław Nieradko",
)

#set text(font: "Lato")

#set par(justify: true)

#set ref(supplement: it => {
  it.supplement.text.replace("Table", "Tabela")
})

#show figure.caption: it => [
  #if it.supplement == [Figure] [
    Rysunek  #it.counter.display()#it.separator #it.body
  ] else if it.supplement == [Table] [
    Tabela  #it.counter.display()#it.separator #it.body
  ] else [
    #it.supplement #it.numbering#it.separator #it.body
  ]
]

#page[
  #align(right)[
    #stack(
      image("imgs/pg-logo.jpg", width: 35%)
    )
  ]

  #align(horizon + center)[
    #stack(
      dir: ttb,
      text(size: 36pt, weight: "semibold")[
        #stack(
          dir: ltr,
          image("imgs/tank-icon.svg", height: 36pt),
          h(10pt),
          text[Tanks]
        )
      ],
      v(15pt),
      text(size: 14pt, weight: "semibold")[Dokumentacja projektu zespołowego na przedmiot Przetwarzanie Rozproszone],
      v(25pt),
      grid(
        columns: (auto, auto, auto),
        column-gutter: 25pt,
        row-gutter: 10pt,
        [Bartłomiej Krawisz], [Jakub Bronowski],  [Stanisław Nieradko],
        [193319], [193208], [193044]
      ),
      v(100pt),
      image("imgs/logo.png"),
    )
  ]
]

#page[
  #outline(
    title: "Spis treści",
    indent: 20pt,
  )
]

== 1. Wprowadzenie

Celem projektu było stworzenie gry sieciowej czasu rzeczywistego, która będzie symulacją bitwy czołgów obsługującą kilku graczy. Gracze sterują czołgami, które poruszają się po planszy~i strzelają do siebie. Gra kończy się zwycięstwem gracza, który zdoła pokonać resztę graczy.

Projekt zaimplementowaliśmy w języku Python bez wykorzystania zewnętrznych bibliotek. Gra działa w trybie serwer-klient, gdzie serwer zarządza grą a klienci sterują czołgami. Zarówno serwer jak~i klient wykorzystują natywną Pythonowi współbieżność opartą o wątki (wbudowana biblioteka `threading`) w celu płynnej oraz jednoczesnej komunikacji sieciowej oraz obsługi gry.

== 2. Model komunikacji

Gra wykorzystuje protokół TCP do komunikacji między klientami a serwerem. Wybór ten został dokonany \ ze względu na wystarczającą wydajność względem UDP~i prostotę implementacji. Podczas testowania eksperymentalnej implementacji opartej o UDP zauważyliśmy, że różnice w opóźnieniach były na tyle małe, że nie wpływały one znacznie na rozgrywkę.

Do serializowania zdarzeń wybraliśmy format JSON z uwagi na czytelność danych oraz nieograniczone możliwości rozbudowy. Każde ze zdarzeń jest zakończone znakiem `NULL` celem odseparowania ich od siebie.

// Jeżeli możemy zastosować JSON to można by dodać JSON Schema w celu poprawy czytelności~i zrozumienia danych

#figure(
  table(
    columns: (1fr, 1fr, 5fr),
    table.header(
      [Nazwa], [Typ], [Opis]
    ),
    `event_type`, "str", box[
      Typ zdarzenia (możliwe typy zdarzenia: `connect`, `refuse`, `ping`, `pong`, `setPlayerId`, `gameState`, `disconnect`).
    ],
    `time`, "int", "Czas zdarzenia [unix timestamp].",
    `data`, "object", box[Dane zdarzenia (zależne od `event_type`).]
  ),
  caption: "Struktura zdarzenia."
)

#figure(
  table(
    columns: (1fr, 1fr, 5fr),
    table.header([Nazwa], [Typ], [Opis]),
    `serverTime`, "int", "Czas serwera [unix timestamp].",
  ),
  caption: box[Struktura obiektu `data` dla zdarzenia `pong`.]
)

#figure(
  table(
    columns: (1fr, 1fr, 5fr),
    table.header([Nazwa], [Typ], [Opis]),
    `playerId`, "int", "Identyfikator gracza.",
  ),
  caption: box[Struktura obiektu `data` dla zdarzenia `setPlayerId`.]
)

#figure(
  table(
    columns: (1fr, auto, auto, 4fr),
    table.header([Nazwa], [Typ], [Czy opcjonalne?], [Opis]),
    `tanks`, "dic[int, Tank]", "Tak", "Obiekty czołgów.",
    `bullets`, "list[Bullet]", "Tak", "Tablica obiektów pocisków.",
    `map`, "list[Obstacle]", "Tak", "Tablica przeszkód na mapie.",
    `isGameOver`, "bool", "Nie", "Czy gra się zakończyła?",
  ),
  caption: box[Struktura obiektu `data` dla zdarzenia `gameState`.]
)


#page[
  === 2.1. Przykładowe zdarzenia

  #text(size: 11pt)[
    ```json
    // Zdarzenie connect
    {"eventType":"connect","time":1711291958}

    // Zdarzenie refuse
    {"eventType":"refuse","time":1711291958}

    // Zdarzenie ping
    {"eventType":"ping","time":1711293829}

    // Zdarzenie pong
    {"eventType":"pong","time":1711295934,"data":{"serverTime":1711293829}}

    // Zdarzenie setPlayerId
    {"eventType":"setPlayerId","time":1711295934,"data":{"playerId":2}}

    // Zdarzenie gameState
    {
      "eventType": "gameState",
      "time": 1711293829,
      "data": {
          "tanks": {
            "1": {
                "x": 115.2,
                "y": 254.32,
                "direction": 1,
                "speed": 100,
                "score": 50,
                "alive": true
            },
            "2": {
                "x": 34.2,
                "y": 74.32,
                "direction": 0,
                "speed": 0,
                "score": 0,
                "alive": false
            }
          },
          "bullets": [
            {
                "x": 115.2,
                "y": 254.32,
                "direction": 1,
                "speed": 100,
                "playerId": 2
            },
            {
                "x": 34.2,
                "y": 74.32,
                "direction": 0,
                "speed": 0,
                "playerId": 1
            }
          ],
          "map": [{"x":51,"y":24,"type":1},{"x":23,"y":11,"type":3},{"x":0,"y":5,"type":0}],
          "isGameOver": false
      }
    }
    ```
  ]
]

=== 2.2. Diagram sekwencji

#figure(
  image("imgs/sequence.png", height: 95%),
  caption: "Diagram sekwencji."
)

== 3. Opis działania aplikacji

Gracze dołączają do serwera poprzez wysłanie zdarzenia `connect`. Następuje 5-krotna wymiana zdarzeń `ping` (ze strony serwera) oraz `pong` (ze strony użytkownika) w celu ustalenia opóźnienia (z ang. _latency_) połączenia. Następnie serwer przesyła zdarzenie `setPlayerId` (z przypisanym identyfikatorem gracza) oraz zdarzenie `gameState` (z aktualnym stanem oraz mapą gry) do użytkownika. Jeżeli okazałoby się, że serwer jest przepełniony to zostanie przesłane zdarzenie `refuse` zamykające połączenie.

Podczas gry gracze przesyłają lokalny stan swojego czołgu co 100 ms - 1 s (w zależności od ilości lokalnych zmian) w formie zdarzenia `gameState`. Serwer odbiera zdarzenia od wszystkich graczy, rozwiązuje konflikty, łączy je w jeden spójny stan~i przesyła do wszystkich graczy w formie zdarzenia `gameState`. Niniejsza operacja wykonywana jest co 100 ms.

Po spełnieniu warunków zakończenia potyczki (pozostanie jeden gracz na planszy), serwer wysyła zdarzenie `gameState` z `isGameOver` ustawionym na `true`, co informuje klientów o zakończeniu gry. Po 3 s serwer rozpoczyna nową grę pozwalając graczom na dalszą jej kontynuację.

W celu rozłączenia się z serwerem gracz wysyła zdarzenie `disconnect` (w wyniku którego otrzymuje on także zwrotne zdarzenie `disconnect` od serwera jako potwierdzenie). Możliwe jest także otrzymanie takiego zdarzenia w przypadku awarii lub zakończenia pracy serwera (zostaje ono wysłane do wszystkich graczy).

== 4. Potencjalne elementy krytyczne

Najważniejszym, pod względem spójności, elementem gry jest obiekt `gameState` przechowujący jej stan. Celem ochrony jego spójności zastosowaliśmy mechanizm blokady Read-Write, umożliwiający jednoczesny odczyt przez wiele wątków oraz wyłączny zapis przez jeden wątek. Dzięki temu zapewniamy, że stan gry jest zawsze spójny~i niezmienny podczas odczytu.

Kolejnym elementem krytycznym jest obsługa zdarzeń. W celu zapewnienia spójności, zdarzenia są przetwarzane w kolejności ich otrzymania w sposób synchroniczny (po jednym zdarzeniu na raz) przez główny wątek serwera, po pobraniu z kolejki zdarzeń. Dzięki temu zapewniamy, że zdarzenia są przetwarzane w kolejności ich otrzymania~i nie dochodzi do konfliktów.

== 5. Diagram klas

#figure(
  image("imgs/class.png", width: 34%),
  caption: "Diagram klas."
)

#pagebreak()

=== 5.1. Opis klas

Nasza gra składa się z kilku klas, z których najważniejsze to:

#list(
  body-indent: 10pt,
  [`Game` - klasa zarządzająca grą, przechowuje stan gry, obsługuje zdarzenia graczy oraz zarządza wątkami.],
  [`GameState` - klasa przechowująca stan gry, czołgów, pocisków i/lub przeszkód na mapie. W zależności od zdarzenia, obiekt może zawierać wszystkie lub parę z tych elementów. Klasa ta jest używana do przesyłania stanu gry między serwerem a klientami. W całości powinna być wysyłana jedynie podczas dołączania gracza do gry. W każdym innym przypadku, zarówno serwer jak~i klient, przesyłają jedynie różnice między stanami gry, które następnie będą przetwarzane przez klasę `Game` po czym nastąpi modyfikacja stanu gry.],
  [`Tank` - klasa reprezentująca czołg (gracza), przechowuje jego pozycję, kierunek, prędkość, punkty oraz informację o tym, czy czołg jest żywy.],
  [`Bullet` - klasa reprezentująca pocisk, przechowuje jego pozycję, kierunek oraz prędkość. Pociski są tworzone przez czołgi~i poruszają się w zadanym kierunku z zadaną prędkością.],
  [`Obstacle` - klasa reprezentująca przeszkodę na mapie, przechowuje jej pozycję oraz typ. Przeszkody są nieruchome~i niezmienne w czasie gry, przez co nie są przesyłane między serwerem a klientami poza początkiem rozgrywki.]
)
Obiekt `Game` jest wykorzystywany zarówno przez klasy `Client` oraz `Server` w celu zarządzania grą oraz obsługi zdarzeń. Dzięki temu mechanizmy gry są identyczne zarówno po stronie serwerera jak~i gracza. Klasa `Client` jest odpowiedzialna za obsługę komunikacji z serwerem, natomiast klasa `Server` za zarządzanie grą, obsługę zdarzeń oraz komunikację z klientami.

Ze względu na typ gry oraz jej implementację nie zdecydowaliśmy się na wprowadzenie dziedziczenia między klasami, gdyż nie przyniosłoby to żadnych korzyści, a jedynie zwiększyłoby złożoność kodu. W przypadku tego typu projektu kompozycja jest zdecydowanie lepszym rozwiązaniem~i pozwala na większą elastyczność przy wprowadzaniu przyszłych zmian.

== 6. FAQ

=== 6.1. TCP vs UDP

Po przetestowaniu obu protokołów, zdecydowaliśmy się na TCP ze względu na mniejszą ilość problemów związanych z przesyłem danych oraz wystarczającą wydajność, dopasowaną do naszych zastosowań.

Choć UDP jest szybszy, to implementacja gry przy użyciu tego  protokołu wymagałaby dodatkowego nakładu pracy. W przypadku TCP odnotowane opóźnienia względem implementacji korzystającej z UDP były na tyle małe, że nie miały one wpływu na rozgrywkę.

=== 6.2. Jak radzimy sobie z sytuacją, w której klient przestał przesyłać informacje?

W przypadku, gdy klient przestaje przesyłać informacje, serwer po 5 s od ostatniego zdarzenia wysyła zdarzenie `disconnect` w celu rozłączenia klienta. W momencie wysyłania zdarzenia czołg klienta zostaje natychmiast usunięty z gry.

W międzyczasie serwer będzie "symulował" zachowanie czołgu poprzez ekstrapolację przez co najwyżej 0,5~s. Po tym czasie czołg klienta zatrzyma się w wyznaczonym przez ekstrapolację miejscu.

=== 6.3. Jak radzimy sobie z sytuacją, w której serwer przestał przesyłać informacje?

Klient symuluje zachowania czołgów innych graczy dzięki ekstrapolacji (przez maksymalnie 0,5 s). Jeżeli do tego czasu nie uda się przywrócić połączenia z serwerem, klient zakończy grę (lokalnie)~i wyświetli komunikat o problemach z połączeniem.

=== 6.4. Jak radzimy sobie z opóźnieniami przesyłu i/lub zakolejkowanymi wiadomościami?

Serwer wykorzystuje asynchroniczny model przetwarzania zdarzeń, co pozwala na obsługę kolejnych zdarzeń podczas wysyłania~i oczekiwania na ukończenie połączenia. Wszystkie zdarzenia są przetwarzane jedne po drugich, zgodnie z kolejnością ich otrzymania. Dzieje się tak również w przypadku opóźnień po stronie klienta. Tak szybko jak nadejdą opóźnione zdarzenia, zostaną one obsłużone.

Podczas gry, gdy zdarzenia klienta nie dochodzą do serwera, ten ekstrapoluje ruch czołgu (przez 0,5 s) na podstawie ostatnich danych otrzymanych od klienta~i przesyła je do pozostałych graczy. Po oknie ekstrapolacji, ale przed wyrzuceniem gracza z powodu nieaktywności czołg klienta nie porusza się. W przypadku, gdy klient znów zacznie przesyłać dane, serwer cofnie czołg klienta do pozycji sprzed maksymalnie 0,5 s~i zaakceptuje dane od klienta, o ile nie są one starsze niż 0,5 s. W innym wypadku pozycja czołgu klienta będzie taka sama, jak po cofnięciu.

=== 6.5. Jak weryfikujemy, czy dane zostały przesłane w całości?

Dzięki wykorzystaniu JSON'a jako medium przesyłu danych, serwer jest w stanie sprawdzić, czy dane zostały przesłane w całości. W przypadku błędu przesyłu danych serwer odrzuca zdarzenie~i oczekuje na kolejne. Każda wiadomość musi być zakończona znakiem `NULL` w celu odseparowania poszczególnych JSON'ów.

=== 6.6. Jak radzimy sobie z sytuacją, gdy pakiet "zagubi się"~i nie dotrze poprawnie do/z serwera?

TCP zapewnia, że dane zostaną dostarczone w całości~i w odpowiedniej kolejności. W przypadku, gdy pakiet zostanie "zagubiony", TCP ponownie wyśle dane. W przypadku, gdy pakiet nie dotrze do serwera, klient ponownie wyśle dane.

Nawet gdyby w "magiczny" sposób była możliwa utrata pakietu lub dokonalibyśmy zmiany protokołu na UDP to nie będzie to miało  wpływu na rozgrywkę. Częstość wysyłania danych jest na tyle duża (100 ms), żeby nie powodowało to żadnych problemów.