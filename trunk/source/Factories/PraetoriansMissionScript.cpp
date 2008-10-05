#include "PraetoriansMissionScript.h"
#include "../Kernel/Gateway.h"
#include "TerrainVisuals.h"
#include "WorldVisuals.h"
#include "../Databases/ModelDatabase.h"
#include "../Controllers/VillageModelController.h"
#include "../Nodes/TransformGroup.h"

PraetoriansMissionScript::PraetoriansMissionScript()
{
  objectTypes.insertKeyAndValue("golondrina", "GOLONDRINA");
  objectTypes.insertKeyAndValue("gorrion", "GORRION");
  objectTypes.insertKeyAndValue("ke_ardilla", "ARDILLA");
  objectTypes.insertKeyAndValue("cuervo", "CUERVO");
  objectTypes.insertKeyAndValue("mirlo", "MIRLO");
  objectTypes.insertKeyAndValue("ke_ciervo_base", "CIERVOMIEDOSO");
  objectTypes.insertKeyAndValue("ke_aldeano_base", "PALETO1");
  objectTypes.insertKeyAndValue("ke_aldeano_tipo2_base", "PALETO2");
  objectTypes.insertKeyAndValue("ke_aldeana_base", "PALETO3");
  objectTypes.insertKeyAndValue("ke_herrero_base", "PALETO4");
  objectTypes.insertKeyAndValue("ke_aldeana_tipo2_base", "PALETO5");
  objectTypes.insertKeyAndValue("kn_aldeana_base", "ALDEANO_N1");
  objectTypes.insertKeyAndValue("kn_aldeana_tipo2_base", "ALDEANO_N2");
  objectTypes.insertKeyAndValue("kn_aldeano_base", "ALDEANO_N3");
  objectTypes.insertKeyAndValue("kn_aldeano_tipo2_base", "ALDEANO_N4");
  objectTypes.insertKeyAndValue("kn_lenador_base", "ALDEANO_N5");
  objectTypes.insertKeyAndValue("kd_aldeana_base", "ALDEANO_D1");
  objectTypes.insertKeyAndValue("kd_aldeana_tipo2_base", "ALDEANO_D2");
  objectTypes.insertKeyAndValue("kd_aldeano_base", "ALDEANO_D3");
  objectTypes.insertKeyAndValue("kd_aldeano_tipo2_base", "ALDEANO_D4");
  objectTypes.insertKeyAndValue("kd_cubos_base", "ALDEANO_D5");
  objectTypes.insertKeyAndValue("ke_mabras_banco", "MABRAS");
  objectTypes.insertKeyAndValue("ke_sardinas_banco", "SARDINAS");
  objectTypes.insertKeyAndValue("ke_serranillos_banco", "SERRANILLOS");
  objectTypes.insertKeyAndValue("ke_alce", "ALCE_CORNUDO");
  objectTypes.insertKeyAndValue("ke_alce_2_base", "ALCE");
  objectTypes.insertKeyAndValue("ke_jabali_base", "JABALI");
  objectTypes.insertKeyAndValue("ke_oveja_1_base", "OVEJA");
  objectTypes.insertKeyAndValue("ke_oveja_2_base", "OVEJA2");
  objectTypes.insertKeyAndValue("ke_oveja_3_base", "OVEJA3");
  objectTypes.insertKeyAndValue("ke_vaca_1_base", "VACA");
  objectTypes.insertKeyAndValue("ke_vaca_2_base", "VACA2");
  objectTypes.insertKeyAndValue("ke_vaca_3_base", "VACA3");
  objectTypes.insertKeyAndValue("ke_conejo_base", "CONEJO");
  objectTypes.insertKeyAndValue("ke_rata_base", "RATA");
}

PraetoriansMissionScript::~PraetoriansMissionScript()
{
}

bool PraetoriansMissionScript::exportData(const char* projectName)
{
  VillageModelController* villageController;
  ModelDatabase* mdatabase;
  WorldVisuals* wvisuals;
  WorldObject* wobject;
  Tuple3f vpos;
  const char* vname;
  string critterType;
  String path;
  string pn;
  Tuple2i area;
  Array <Tuple2f> posarray(10);
  
  wvisuals = Gateway::getWorldVisuals();
  mdatabase = Gateway::getVillageDatabase();
  path = Gateway::getExportPath();
  area = Gateway::getTerrainVisuals()->getArea();
  pn = projectName;
  ofstream f((path + "Misiones/" + projectName + ".MSS").getBytes());
  
  if (!f.is_open())
    return Logger::writeErrorLog("Could not export mission script");
    
  f << "{" << endl;
  f << "  *ESCENARIO" << endl;
  f << "  {" << endl;
  f << "    *VISUAL \"Mapas\\" + pn + ".PVE\"" << endl;
  f << "    *LOGICO \"Mapas\\" + pn + ".MLG\"" << endl;
  f << "    *OBJETOS \"Mapas\\" + pn + ".MOB\"" << endl;
  f << "    *PRADERAS \"Mapas\\" + pn + ".PRA\"" << endl;
  f << "    *AGUA \"Mapas\\" + pn + ".H2O\"" << endl;
  f << "    *ZONAS_ACC \"Mapas\\MP_4_1.ZAC\"" /*+ pn + ".ZAC\""*/<< endl;
  f << "    *DATOS_IA \"Mapas\\MP_4_1.PIA\"" /*+ pn + ".PIA\""*/<< endl;
  f << "    //*MINIMAPA \"Mapas\\DESCRIPCION\\" + pn + ".TGA\"" << endl;
  f << (String("    *CONFIG_TERRENO \"") + Gateway::getMapDescriptor().mapPastureType + "\"").getBytes() << endl;
  f << "    *ACTITUD_CPU \"ActitudCPU\\Skirmish.dat\"" << endl;
  f << (String("    *MALLA ") + area.x + " " + area.y).getBytes() << endl;
  f << "  }" << endl;
  
  f << endl;
  
  f << "  *MAPEADOR" << endl;
  f << "  {" << endl;
  f << "     *CAMARA 90.1853 291.769 69.185 50 0 88 2000 0.1" << endl;
  f << "     *FLAGS 0x42b288" << endl;
  f << "  }" << endl;
  
  f << endl;
  
  f << "  *COLORES_MAPEADOR" << endl;
  f << "  {" << endl;
  f << "    *MALLA_IMPAR 0xff408080" << endl;
  f << "    *MALLA_PAR 0xff808080" << endl;
  f << "    *MALLA_BORDE 0x7fc0c0c0" << endl;
  f << "    *TEXTURE_BORDE 0x7f00ff00" << endl;
  f << "    *CASILLA_NOSEL 0x7fffffff" << endl;
  f << "    *VERT_DIAG 0xffc0c0c0" << endl;
  f << "    *VERT_POND 0xff00ff00" << endl;
  f << "    *SPLINE_NOSEL 0xff400000" << endl;
  f << "    *STR_ENTRADA 0xff000000" << endl;
  f << "    *IA_PTO_CONTROL 0xff000000" << endl;
  f << "  }" << endl;
  
  f << endl;
  
  f << string("  *NOMBRE_MISION \"") + pn + "\"" << endl;
  
  f << endl;
  
  f << "  *MUSICA" << endl;
  f << "  {" << endl;
  f << "    *TIPO \"SA01\"" << endl;
  f << "  }" << endl;
  f << "  *MUSICA_BATALLA" << endl;
  f << "  {" << endl;
  f << "    *TIPO \"SB01\"" << endl;
  f << "  }" << endl;
  
  f << endl;
  
  f << (String("  *MAX_PLAYERS ") + (int)Gateway::getMapDescriptor().maxPlayers).getBytes() << endl;
  posarray = Gateway::getMapDescriptor().playerPositions;
  for (unsigned int i = 0; i < Gateway::getMapDescriptor().playerPositions.length(); i++)
    f << (String("  *POS_INICIAL ") + posarray[i].y + " " + posarray[i].x).getBytes() << endl;
    
  f << endl;
  
  f << "  *CIVILIZACION" << endl;
  f << "  {" << endl;
  f << "    *NOMBRE \"NEUTRAL\"" << endl;
  f << "    *RAZA \"BARBAROS\"" << endl;
  f << "    *COLOR 7" << endl;
  f << "    *NEUTRAL" << endl;
  f << "  }" << endl;
  
  f << endl;
  
  mdatabase = Gateway::getVillageDatabase();
  for (unsigned int i = 0; i < mdatabase->getModelControllerCount(); i++)
  {
    villageController = (VillageModelController*) mdatabase->getModelController(i);
    vpos = mdatabase->getModelController(i)->getPosition();
    vname = mdatabase->getModelController(i)->getTransformGroup()->getName();
    f << "  *ESTRUCTURA" << endl;
    f << "  {" << endl;
    f << (String("    *DEFINICION \"") + vname + "\"").getBytes() << endl;
    f << (String("    *POSICION ") + vpos.z + " " + vpos.y + " " + vpos.x).getBytes() << endl;
    f << "    *ORIENTACION 0" << endl;
    f << string("    *VISUAL \"") + vname + "\"" << endl;
    f << (String("    *HABITANTES ") + int(villageController->getPopulation())).getBytes() << endl;
    f << (String("    *MAX_HABITANTES ") + int(villageController->getMaxPopulation())).getBytes() << endl;
    f << "  }" << endl;
  }
  
  f << endl;
  
  for (unsigned int i = 0; i < wvisuals->getObjectCount(); i++)
  {
    wobject = wvisuals->getObject(i);
    
    if (wobject->type != WorldObjectTypes::CRITTER)
      continue;
      
    if (!objectTypes.findValue(wobject->name.getBytes(), critterType))
      continue;
      
    vpos = wobject->position;
    vname = wobject->name;
    
    f << "  *CRITTER" << endl;
    f << "  {" << endl;
    f << string("    *TIPO \"") + critterType + "\"" << endl;
    f << (String("    *POSICION ") + vpos.z + " " + vpos.y + " " + vpos.x).getBytes() << endl;
    f << "    *ORIENTACION 0" << endl;
    f << "  }" << endl;
  }
  
  f << endl;
  
  f << "  *PUNTOS_CAMARA" << endl;
  f << "  {" << endl;
  f << "    *POS \"TOMA_0\" 22.4033 63.2512 72.5494 21.391 0 73.2304" << endl;
  f << "    *POS \"TOMA_1\" 173.857 27.6 -7.58563 163.734 0 15.6362" << endl;
  f << "    *POS \"TOMA_2\" 172.535 15 24.6453 162.976 0 34.2043" << endl;
  f << "    *POS \"TOMA_3\" 136.022 15 70.4689 126.463 0 80.0279" << endl;
  f << "    *POS \"TOMA_4\" 27.984 15 1.31899 18.425 0 10.878" << endl;
  f << "    *POS \"TOMA_5\" 12.5749 15 77.4725 3.01588 0 87.0315" << endl;
  f << "  }" << endl;
  
  f << "}" << endl;
  
  f.close();
  
//*ESTRUCTURA
//{
// *DEFINICION "PESE_1"
// *POSICION 23.5 0 133.5
// *ORIENTACION 0
// *VISUAL "PESE_1"
// *HABITANTES 600
// *MAX_HABITANTES 600
//}
//*ESTRUCTURA
//{
// *DEFINICION "PESW_1"
// *POSICION 76.5 -0.00302124 133.5
// *ORIENTACION 0
// *FLAGS 1
// *VISUAL "PESW_1"
// *HABITANTES 150
// *MAX_HABITANTES 400
// *VEL_REGENERACION 5
// *PUNTO_ANEXO -5.80005 0 2.72612
//}
//*ESTRUCTURA
//{
// *DEFINICION "PESW_1"
// *POSICION 133.5 0 117.5
// *ORIENTACION 0
// *VISUAL "PESW_1"
// *HABITANTES 600
// *MAX_HABITANTES 600
// *PUNTO_ENTRADA 0.0837555 -0.00301933 -4.51111
// *PUNTO_SALIDA 0.121231 -0.00301933 -4.52985
// *PUNTO_ANEXO -3.76587 -0.00302124 5.15492
//}
//*ESTRUCTURA
//{
// *DEFINICION "PENW_1"
// *POSICION 133.5 -0.00302124 63.5
// *ORIENTACION 0
// *VISUAL "PENW_1"
// *HABITANTES 150
// *MAX_HABITANTES 400
// *VEL_REGENERACION 5
// *PUNTO_ANEXO 2.87982 0 6.92342
//}
//*ESTRUCTURA
//{
// *DEFINICION "PENW_1"
// *POSICION 115.5 -0.00302124 6.5
// *ORIENTACION 0
// *VISUAL "PENW_1"
// *HABITANTES 600
// *MAX_HABITANTES 600
// *PUNTO_ANEXO 6.80603 -0.00302124 3.19924
//}
//*ESTRUCTURA
//{
// *DEFINICION "PENE_1"
// *POSICION 63.5 -0.00302124 6.5
// *ORIENTACION 0
// *VISUAL "PENE_1"
// *HABITANTES 150
// *MAX_HABITANTES 400
// *VEL_REGENERACION 5
// *PUNTO_ANEXO 5.26588 0 -3.13383
//}
//*ESTRUCTURA
//{
// *DEFINICION "PENE_1"
// *POSICION 6.5 -0.00302124 24.5
// *ORIENTACION 0
// *VISUAL "PENE_1"
// *HABITANTES 600
// *MAX_HABITANTES 600
// *PUNTO_ANEXO 3.45133 0 -5.43999
//}
//*ESTRUCTURA
//{
// *DEFINICION "PESE_1"
// *POSICION 6.5 -0.00302124 76.5
// *ORIENTACION 0
// *VISUAL "PESE_1"
// *HABITANTES 150
// *MAX_HABITANTES 400
// *VEL_REGENERACION 5
// *PUNTO_ANEXO -2.40046 0 -6.66415
//}
//*ESTRUCTURA
//{
// *DEFINICION "PES_1"
// *POSICION 70.5 0 73.5
// *ORIENTACION 0
// *VISUAL "PES_1"
// *VEL_REGENERACION 3
// *PUNTO_ANEXO -0.225975 -0.00302124 7.40271
//}

  return true;
}