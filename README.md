qtden release yaptıktan sonra release dosyasına giriyoruz.
dosyadaki exe hariç her şeyi siliyoruz.
daha sonra qt nin c deki C:\Qt\6.8.0 konumuna geliyoruz.
konumda release yapma şeklimize göre sürüm seçiyoruz(mingw_64/llvm_mingv_64/android_x86...)
onun içindeki bin doyasına giriyoruz.
bu klasörde shift+sağ click yapıp powershell veya cmd den komut ekranını açıyoruz.
oradan release.exe dosyasının konumunu alıyoruz.


PS C:\Qt\6.8.0\mingw_64\bin> .\windeployqt --qmldir "C:\Qt\6.8.0\mingw_64\qml"
 "C:\Users\PC_6270\Desktop\Enes\QtAndroidInterfaceProject\inteface\build\Desktop_Qt_6_8_0_MinGW_64_bit-Release\release\inteface.exe"
qml dosyasını ise aynı bin'deki qml dosya yolunu alıyoruz.
daha sonra exemizi çalıştırabiliriz.
(Android için denenmedi)

video: https://www.youtube.com/watch?v=rFHPOZoqzcg


-Uygulama Klasöründe oluşan Tile klasörüne Dışarıdan  OFFLİNE kullanım için tile yükleyebilirsiniz.
-Bunu yapabilmek için öncelikle tile ları OSM formatına çevirmeniz gerekmektedir.
-OSM FORMATINA çevirmek için harici yazmış olduğum https://github.com/enescankaya/Tile-Converter-from-BingSattelite-to-the-OSM.git kullanabilirsiniz.
-tile ları OSM formatına çevirdikten sonra uygulama içerisinde bulunan tiles klasörüne görüntüleri uygun formatta ekleyin(sadece görüntü olacak alt_dir yok)
