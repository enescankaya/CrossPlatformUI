# Hava Aracı Kontrol Uygulaması

Bu uygulama, TCP ve UDP üzerinden MAVLink protokolü kullanarak hava aracı ile iletişim kuran ve hava aracının kontrolünü sağlayan bir yazılımdır. Qt 6.8 v1 C++ kullanılarak geliştirilmiş olup, profesyonel bir uçuş kontrol arayüzü sunmaktadır. Uygulama, kullanıcı dostu arayüzüyle hava aracı parametrelerini kontrol etmek ve komutları iletmek için tasarlanmıştır.

---

## Özellikler

- **MAVLink İletişimi**: TCP ve UDP üzerinden MAVLink protokolü kullanarak hava aracı ile haberleşir.
- **Arayüz Kontrolü**: Kullanıcı, GUI üzerinden hava aracı parametrelerini görüntüleyebilir ve kontrol edebilir.
- **Offline Tile Desteği**: Harita görüntüleri için offline (çevrimdışı) kullanım desteği sunar.
- **Tile Converter**: Harici araç ile Bing Satellite formatındaki tile'ları OSM formatına dönüştürme desteği sağlar.

---

## Kurulum Adımları

### 1. Qt Projesi ve Release Yapma

1. **Uygulama Dosyalarının Hazırlanması**:
   - Uygulamanızın `Release` klasörüne gidin.
   - **Release** dosyasındaki `.exe` dışında her şeyi silin.
   - Qt kurulum klasörüne gidin (örneğin: `C:\Qt\6.8.0`).

2. **Release Komutunu Kullanma**:
   - Qt kurulumunda, `bin` klasörüne gidin. Örneğin:
     - `C:\Qt\6.8.0\mingw_64\bin`
   - Burada `Shift + Sağ Tıklayıp PowerShell veya CMD açın`.
   - `windeployqt` komutunu kullanarak gerekli bağımlılıkları oluşturun:
   
     ```bash
     .\windeployqt --qmldir "C:\Qt\6.8.0\mingw_64\qml" "C:\Users\PC_6270\Desktop\Enes\QtAndroidInterfaceProject\inteface\build\Desktop_Qt_6_8_0_MinGW_64_bit-Release\release\inteface.exe"
     ```

3. **QML Dosyalarını Eklemek**:
   - `qml` dosyasını, `bin` içindeki uygun `qml` dosya yoluna yerleştirin.

4. **Uygulamayı Çalıştırma**:
   - Gerekli tüm dosyalar ve bağımlılıklar yerleştirildikten sonra, `.exe` dosyasını çalıştırabilirsiniz.

---

## Offline Tile Kullanımı

- **Tile'lar**: Uygulama klasöründe oluşan `Tile` klasörüne offline kullanım için tile'lar yükleyebilirsiniz. 
- **Tile Formatı**: Uygulama, OSM formatında tile'lar kullanmaktadır. Bu formatta tile'ları yüklemek için aşağıdaki adımları takip edebilirsiniz:
  
  1. **Tile Converter Kullanma**:
     - Bing Satellite tile'larını OSM formatına çevirmek için aşağıdaki GitHub reposunu kullanabilirsiniz:
       - [Tile Converter from Bing Satellite to OSM](https://github.com/enescankaya/Tile-Converter-from-BingSattelite-to-the-OSM.git)
     
  2. **Tile'ları Yükleme**:
     - OSM formatına çevirdiğiniz tile'ları uygulama içindeki `tiles` klasörüne uygun formatta ekleyin.
     - **Not**: Tile'larda sadece görüntü dosyalarını ekleyin, alt klasörler bulunmamalıdır.

---

## Kullanım

- **Uygulama Başlatma**: Uygulama başlatıldığında, MAVLink üzerinden hava aracıyla iletişim kurarak temel komutları gönderebilirsiniz. 
- **Harita Görüntüleme**: Offline tile desteği sayesinde, internet bağlantısı olmadan harita görüntüleme işlemini gerçekleştirebilirsiniz.

---

## Lisans

Bu proje, [MIT Lisansı](LICENSE) altında lisanslanmıştır.
