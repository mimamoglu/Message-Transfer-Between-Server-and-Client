# Message-Transfer-Between-Server-and-Client
Message transmission between server and clients is implemented using the message passing method. The server receives incoming messages and forwards them to other clients. The client implements message sending and receiving functions using multithreading.


##Sunucu-İstemci Arasında Mesaj Aktarımı##

Sunucu istemciler arasında mesaj iletimini message passing yöntemi ile gerçekleştirir. Sunucu, gelen mesajları alır ve diğer istemcilere iletir. İstemci, mesaj gönderme ve almayı fonksiyonlarını multithreading ile gerçekleştirir.

Başlangıç
Yerel makinenizde çalıştırmak için aşağıdaki adımları izleyin.

Gereksinimler
GCC veya C derleyici
Linux işletim sistemi
Kurulum
Dosyaları indirin:
İndirmiş olduğunuz dosyaları aynı klasör içerisine çıkartın.
Sunucu uygulamasını derleyin:
gcc server.c -o server
İstemci uygulamasını derleyin:
gcc client.c -o client
Çalıştırma
Sunucuyu başlatın:
./server
İstemciyi başlatın:
./client <istemci_kimlik>
Not: <istemci_kimlik> yerine istemcinin kimlik numarasını belirleyin. Her bir istemcinin benzersiz bir kimliği olmalıdır.

Kullanım
Sunucu başlatıldığında, istemciler sunucuya bağlanır ve mesajlaşmaya başlayabilir.

İstemcilerin mesaj göndermek için "Kime mesaj göndermek istiyorsunuz? (İstemci kimliği):" şeklinde bir giriş yapmaları gerekmektedir. Mesajı alacak istemci kimliğini girin. Daha sonrasında kullanıcan "Mesajınızı girin:" şeklinde gönderilmek istenen mesaj istemciden alınır.

Mesaj gönderildikten sonra, sunucu gelen mesajı diğer istemciye iletir.

İstemciler, diğer istemcilerden sunucu aracılığıyla gelen mesajları "Mesaj alındı: <mesaj_metni>" şeklinde görecektir.

İstemci programının sonlanması ve sunucuyla bağlantının kesilmesi için "Kime mesaj göndermek istiyorsunuz? (İstemci kimliği):" kısmına -1 inputu girerek "Mesajınızı girin:"  kısmına ise "exit" mesajını girerek mevcut istemciyi sonlandırabilirler
