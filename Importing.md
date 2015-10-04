First, realize that the plantillas.xml contains example projects. Those examples are there for you, the user.


Now, once you have exported your map you will need to add some data to the plantillas.xml
file. The new files will exist in the "Export" folder. We must tell the map editor that we want to use these files. Here is an example:
  * Texturas name = "kwantum.PTE"
  * Objetos name = "kwantum.MOB"
  * Visual name = "kwantum.PVE"
  * Praderas name = "kwantum.PRA"
  * Logica name = "kwantum.MLG"
  * Agua name = "kwantum.H2O"


You will also get two BMP files so that you can edit them in Photoshop, or some other image editing software. If you want to use the BMP files, then do something like this example:

  * Texturas name = "kwantum.PTE"
  * Objetos name = "kwantum.MOB"
  * Visual name = "kwantum.bmp"
  * Colores name = "kwantum\_color.bmp"
  * Praderas name = "kwantum.PRA"
  * Logica name = "kwantum.MLG"
  * Agua name = "kwantum.H2O"

Now restart the map editor and load your project.


Note:
  * Once you export villages you cannot import them back again.
  * Once you export critters you cannot import them back again.
  * Once you export player positions you cannot import them back again.