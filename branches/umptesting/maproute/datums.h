/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#ifndef Datum_listH
#define Datum_listH

struct Datum_struct
{
  char name[90];
  char country[90];
  char code[6];

  char elipsoid[4];
  int  dX,dY,dZ;
  int  d1,d2,d3;
};

struct Elipsoid_struct
{
	char name[27];
	char code[3];
	long double majorAxis;
	long double flattering;
};

Elipsoid_struct Elipsoid_list[] =
{ /*1234567890123456789012345*/
  {"Airy 1830","AA",  6377563.396, 299.3249646},
  {"Modified Airy","AM",  6377340.189, 299.3249646},
  {"Australian National","AN",  6378160.000, 298.25},
  {"Bessel 1841 (Namibia)","BN",  6377483.865, 299.1528128},
  {"Bessel 1841","BR",  6377397.155, 299.1528128},
  {"Clarke 1866","CC",  6378206.4,  294.9786982},
  {"Clarke 1880","CD",  6378249.145, 293.465},
  {"Everest 1830","EA",  6377276.345, 300.8017},
  {"Everest (Sabah & Sarawak)","EB",  6377298.556, 300.8017},
  {"Everest 1956","EC",  6377301.243, 300.8017},
  {"Everest 1969","ED",  6377295.664, 300.8017},
  {"Everest 1948","EE",  6377304.063, 300.8017},
  {"Everest (Pakistan)","EF",  6377309.613, 300.8017},
  {"Modified Fischer 1960","FA",  6378155.000, 298.3},
  {"Fischer 1960","FB",  6378166.000, 298.3},
  {"Fischer 1968","FI",  6378150.000, 298.3},
  {"Hayford","HA",  6378388.000, 297.0},
  {"Helmert 1906","HE",  6378200.000, 298.3},
  {"Hough 1960","HO",  6378270.000, 297.0},
  {"Indonesian 1974","ID",  6378160.000, 298.247},
  {"International","IN",  6378388.000, 297.0},
  {"Krassovsky","KA",  6378245.000, 298.3},
  {"GRS 1980","RF",  6378137.000, 298.257222101},
  {"South American 1969","SA",  6378160.000, 298.25},
  {"WGS 60","WA",  6378165.000, 298.3},
  {"WGS 66","WB",  6378145.000, 298.25},
  {"WGS 72","WD",  6378135.000, 298.26},
  {"WGS 84","WE",  6378137.000, 298.257223563},
  {"END","",  0, 0}
};

Datum_struct Datum_list[] =
{
  {"Adindan","Ethiopia","ADI-A","CD",-165,-11,206,3,3,3},
  {"Adindan","Sudan","ADI-B","CD",-161,-14,205, 3,5,3},
  {"Adindan","Mali","ADI-C"," CD",-123,-20,220, 25,25,25},
  {"Adindan","Senegal","ADI-D","CD", -128,-18,224, 25,25,25},
  {"Adindan","Burkina Faso","ADI-E","CD", -118,-14,218, 25,25,25},
  {"Adindan","Cameroon","ADI-F","CD", -134,-2,210, 25,25,25},
  {"Adindan","MEAN FOR Ethiopia, Sudan","ADI-M","CD", -166,-15,204, 5,5,3},
  {"Afgooye","Somalia","AFG","KA",-43,-163,45, 25,25,25},
  {"Antigua Island Astro 1943","Antigua (Leeward Islands)","AIA","CD", -270,13,62, 25,25,25},
  {"Ain el Abd 1970","Bahrain","AIN-A","IN", -150,-250,-1, 25,25,25},
  {"Ain el Abd 1970","Saudi Arabia","AIN-B","IN", -143,-236,7, 10,10,10},
  {"American Samoa 1962","American Samoa Islands","AMA","CC", -115,118,426, 25,25,25},
  {"Anna 1 Astro 1965","Cocos Islands","ANO","AN", -491,-22,435, 25,25,25},
  {"Arc 1950","Botswana","ARF-A", "CD", -138,-105,-289, 3,5,3},
  {"Arc 1950","Lesotho","ARF-B", "CD", -125,-108,-295, 3,3,8},
  {"Arc 1950","Malawi","ARF-C", "CD", -161,-73,-317, 9,24,8},
  {"Arc 1950","Swaziland","ARF-D", "CD", -134,-105,-295, 15,15,15},
  {"Arc 1950","Zaire","ARF-E", "CD", -169,-19,-278, 25,25,25},
  {"Arc 1950","Zambia","ARF-F", "CD", -147,-74,-283, 21,21,27},
  {"Arc 1950","Zimbabwe","ARF-G", "CD", -142,-96,-293, 5,8,11},
  {"Arc 1950","Burundi","ARF-H", "CD", -153,-5,-292, 20,20,20},
  {"Arc 1950","MEAN FOR ARC50 (except Burundi)","ARF-M", "CD", -143,-90,-294, 20,33,20},
  {"Arc 1960","MEAN FOR Kenya,Tanzania","ARS", "CD", -160,-6,-302, 20,20,20},
  {"Ascension Island 1958","Ascension Island","ASC", "IN", -205,107,53, 25,25,25},
  {"Montserrat Island","Astro 1958,Montserrat (Leeward Islands)","ASM","CD", 174,359,365, 25,25,25},
  {"Astronomical Station 1952","Marcus Island","ASQ", "IN", 124,-234,-25, 25,25,25},
  {"Astro Beacon E 1945","Iwo Jima","ATF", "IN", 145,75,-272, 25,25,25},
  {"Australian Geodetic 1966","Australia, Tasmania","AUA", "AN", -133,-48,148, 3,3,3},
  {"Australian Geodetic 1984","Australia, Tasmania","AUG", "AN", -134,-48,149, 2,2,2},
  {"Djakarta (Batavia)","Indonesia (Sumatra)","BAT", "BR", -377,681,-50, 3,3,3},
  {"Bermuda 1957","Bermuda","BER", "CC", -73,213,296, 20,20,20},
  {"Bissau","Guinea-Bissau","BID", "IN", -173,253,27, 25,25,25},
  {"Bogota Observatory","Colombia","BOO", "IN", 307,304,-318, 6,5,6},
  {"Bukit Rimpah","Indonesia (Bangka & Belitung Islands)","BUR", "BR", -384,664,-48, 0,0,0},
  {"Cape Canavera","Bahamas, Florida","CAC", "CC", -2,151,181, 3,3,3},
  {"Campo Inchauspe","Argentina","CAI", "IN", -148,136,90, 5,5,5},
  {"Canton Astro 1966","Phoenix Islands","CAO", "IN", 298,-304,-375, 15,15,15},
  {"Cape","South Africa","CAP", "CD", -136,-108,-292, 3,6,6},
  {"Camp Area Astro","Antarctica (McMurdo Camp Area)","CAZ", "IN", -104,-129,239, 0,0,0},
  {"S-JTSK","Czechoslavakia (Prior 1 JAN 1993)","CCD", "BR", 589,76,480, 4,2,3},
  {"Carthage","Tunisia","CGE", "CD", -263,6,431, 6,9,8},
  {"Chatham Island Astro 1971","New Zealand (Chatham Island)","CHI", "IN", 175,-38,113, 15,15,15},
  {"Chua Astro","Paraguay","CHU", "IN", -134,229,-29, 6,9,5},
  {"Corrego Alegre","Brazil","COA", "IN", -206,172,-6, 5,3,5},
  {"Dabola","Guinea","DAL", "CD", -83,37,124, 15,15,15},
  {"Deception Island","Deception Island, Antarctia","DID", "CD", 260,12,-147, 20,20,20},
  {"GUX 1 Astro","Guadalcanal Island","DOB", "IN", 252,-209,-751, 25,25,25},
  {"Easter Island 1967","Easter Island","EAS", "IN", 211,147,111, 25,25,25},
  {"Wake-Eniwetok 1960","Marshall Islands","ENW", "HO", 102,52,-38, 3,3,3},
  {"Estonia,","Coordinate System 1937,Estonia","EST", "BN", 374,150,588, 2,3,3},
  {"European 1950","MEAN FOR NW Europe","EUR-A", "IN", -87,-96,-120, 3,3,3},
  {"European 1950","Greece","EUR-B", "IN", -84,-95,-130, 25,25,25},
  {"European 1950","Finland, Norway","EUR-C", "IN", -87,-95,-120, 3,5,3},
  {"European 1950","Portugal, Spain","EUR-D", "IN", -84,-107,-120, 5,6,3},
  {"European 1950","Cyprus","EUR-E", "IN", -104,-101,-140, 15,15,15},
  {"European 1950","Egypt","EUR-F", "IN", -130,-117,-151, 6,8,8},
  {"European 1950","England, Channel Islands, Scotland, Shetland Islands","EUR-G", "IN", -86,-96,-120, 3,3,3},
  {"European 1950","Iran","EUR-H", "IN", -117,-132,-164, 9,12,11},
  {"European 1950","Italy (Sardinia)","EUR-I", "IN", -97,-103,-120, 25,25,25},
  {"European 1950","Italy (Sicily)","EUR-J", "IN", -97,-88,-135, 20,20,20},
  {"European 1950","England, Ireland, Scotland, Shetland Islands","EUR-K", "IN", -86,-96,-120, 3,3,3},
  {"European 1950","Malta","EUR-L", "IN", -107,-88,-149, 25,25,25},
  {"European 1950","MEAN FOR ED50 W Europe","EUR-M", "IN", -87,-98,-121, 3,8,5},
  {"European 1950","MEAN FOR ED50 Near East","EUR-S", "IN", -103,-106,-141, 3,3,3},
  {"European 1950","Tunisia","EUR-T", "IN", -112,-77,-145, 25,25,25},
  {"European 1979","MEAN FOR ED79","EUS", "IN", -86,-98,-119, 3,3,3},
  {"Oman","Oman","FAH", "CD", -346,-1,224, 3,3,9},
  {"Observatorio Meteorologico 1939","Azores (Corvo & Flores Islands)","FLO", "IN", -425,-169,81, 20,20,20},
  {"Fort Thomas 1955","Nevis, St. Kitts (Leeward Islands)","FOT", "CD", -7,215,225, 25,25,25},
  {"Gan 1970","Republic of Maldives","GAA", "IN", -133,-321,50, 25,25,25},
  {"Geodetic Datum 1949","New Zealand","GEO", "IN", 84,-22,209, 5,3,5},
  {"DOS 1968","New Georgia Islands (Gizo Island)","GIZ", "IN", 230,-199,-752, 25,25,25},
  {"Graciosa Base SW 1948","Azores (Faial, Graciosa, Pico, Sao Jorge, Terceira)","GRA", "IN", -104,167,-38, 3,3,3},
  {"Gunung Segara","Indonesia (Kalimantan)","GSE", "BR", -403,684,41, 0,0,0},
  {"Guam 1963","Guam","GUA", "CC", -100,-248,259, 3,3,3},
  {"Herat North","Afghanistan","HEN", "IN", -333,-222,114, 0,0,0},
  {"Provisional South Chilean 1963","Chile (South, Near 53�S) (Hito XVIII)","HIT", "IN", 16,196,93, 25,25,25},
  {"Hermannskogel Datum","Croatia,Serbia,Bosnia-Herzegovina,Slovenia","HER", "BN", 653,-212,449, -1,-1,-1},
  {"Hjorsey 1955","Iceland","HJO", "IN", -73,46,-86, 3,3,6},
  {"Hong Kong 1963","Hong Kong","HKD", "IN", -156,-271,-189, 25,25,25},
  {"Hu-Tzu-Shan","Taiwan","HTN", "IN", -637,-549,-203, 15,15,15},
  {"Bellevue (IGN)","Efate & Erromango Islands","IBE", "IN", -127,-769,472, 20,20,20},
  {"Indonesian","Indonesia","IDN", "ID", -24,-15,5, 25,25,25},
  {"Indian (Bangladesh)","Bangladesh","IND-B", "EA", 282,726,254, 10,8,12},
  {"Indian (India)","India, Nepal","IND-I", "EC", 295,736,257, 12,10,15},
  {"Indian (Pakistan)","Pakistan","IND-P", "EF", 283,682,231, 64,0,0},
  {"Indian 1954","Thailand, Vietnam","INF-A", "EA", 217,823,299, 15,6,12},
  {"Indian 1960","Vietnam (Near 16N)","ING-A", "EA", 198,881,317, 25,25,25},
  {"Indian 1960","Vietnam (Con Son Island)","ING-B", "EA", 182,915,344, 25,25,25},
  {"Indian 1975","Thailand","INH-A", "EA", 210,814,289, 3,2,3},
  {"Ireland 1965","Ireland","IRL","AM", 506,-122,611, 3,3,3},
  {"ISTS 061 Astro 1968","South Georgia Islands","ISG", "IN", -794,119,-298, 25,25,25},
  {"ISTS 073 Astro 1969","Diego Garcia","IST", "IN", 208,-435,-229, 25,25,25},
  {"Johnston Island 1961","Johnston Island","JOH", "IN", 189,-79,-202, 25,25,25},
  {"Kandawala","Sri Lanka","KAN","EA", -97,787,86, 20,20,20},
  {"Kertau 1948","West Malaysia & Singapore","KEA", "EE", -11,851,5, 10,8,6},
  {"Kerguelen Island 1949","Kerguelen Island","KEG", "IN", 145,-187,103, 25,25,25},
  {"Korean Geodetic System","South Korea","KOR", "RF", 0,0,0, 2,2,2},
  {"Kusaie Astro 1951","Caroline Islands","KUS", "IN", 647,1777,-1124, 25,25,25},
  {"L. C. 5 Astro 1961","Cayman Brac Island","LCF", "CC", 42,124,147, 25,25,25},
  {"Leigon","Ghana","LEH", "CD", -130,29,364, 2,3,2},
  {"Liberia 1964","Liberia","LIB", "CD", -90,40,88, 15,15,15},
  {"Luzon","Philippines (Excluding Mindanao)","LUZ-A", "CC", -133,-77,-51, 8,11,9},
  {"Luzon","Philippines (Mindanao)","LUZ-B", "CC", -133,-79,-72, 25,25,25},
  {"Massawa","Ethiopia (Eritrea)","MAS", "BR", 639,405,60, 25,25,25},
  {"Merchich","Morocco","MER", "CD", 31,146,47, 5,3,3},
  {"Midway Astro 1961","Midway Islands","MID", "IN", 912,-58,1227, 25,25,25},
  {"Mahe 1971","Mahe Island","MIK", "CD", 41,-220,-134, 25,25,25},
  {"Minna","Cameroon","MIN-A", "CD", -81,-84,115, 25,25,25},
  {"Minna","Nigeria","MIN-B", "CD", -92,-93,122, 3,6,5},
  {"Rome 1940","Italy (Sardinia)","MOD", "IN", -225,-65,9, 25,25,25},
  {"M'Poraloko","Gabon","MPO", "CD", -74,-130,42, 25,25,25},
  {"Viti Levu 1916","Fiji (Viti Levu Island)","MVS","CD", 51,391,-36, 25,25,25},
  {"Nahrwan","Oman (Masirah Island)","NAH-A", "CD", -247,-148,369, 25,25,25},
  {"Nahrwan","United Arab Emirates","NAH-B", "CD", -249,-156,381, 25,25,25},
  {"Nahrwan","Saudi Arabia","NAH-C", "CD", -243,-192,477, 20,20,20},
  {"Naparima BWI","Trinidad & Tobago","NAP", "IN", -10,375,165, 15,15,15},
  {"North American 1983","Alaska (Excluding Aleutian Islands)","NAR-A", "RF", 0,0,0, 2,2,2},
  {"North American 1983","Canada","NAR-B", "RF", 0,0,0, 2,2,2},
  {"North American 1983","CONUS","NAR-C", "RF", 0,0,0, 2,2,2},
  {"North American 1983","Mexico, Central America","NAR-D", "RF", 0,0,0, 2,2,2},
  {"North American 1983","Aleutian Ids","NAR-E", "RF", -2,0,4, 5,2,5},
  {"North American 1983","Hawaii","NAR-H", "RF", 1,1,-1, 2,2,2},
  {"North American 1927","MEAN FOR CONUS (East, Incl LA,MO,MN),","NAS-A", "CC", -9,161,179, 5,5,8},
  {"North American 1927","MEAN FOR CONUS (West, Excl LA,MN,MO),","NAS-B", "CC", -8,159,175, 5,3,3},
  {"North American 1927","MEAN FOR CONUS","NAS-C", "CC", -8,160,176, 5,5,6},
  {"North American 1927","Alaska (Excluding Aleutian Ids)","NAS-D", "CC", -5,135,172, 5,9,5},
  {"North American 1927","MEAN FOR Canada","NAS-E", "CC", -10,158,187, 15,11,6},
  {"North American 1927","Canada (Alberta, British Columbia)","NAS-F", "CC", -7,162,188, 8,8,6},
  {"North American 1927","Canada (New Brunswick, Newfoundland,Nova Scotia,Quebec)","NAS-G", "CC", -22,160,190, 6,6,3},
  {"North American 1927","Canada (Manitoba, Ontario)","NAS-H", "CC", -9,157,184, 9,5,5},
  {"North American 1927","Canada (Northwest Territories, Saskatchewan)","NAS-I", "CC", 4,159,188, 5,5,3},
  {"North American 1927","Canada (Yukon)","NAS-J", "CC", -7,139,181, 5,8,3},
  {"North American 1927","Mexico","NAS-L", "CC", -12,130,190, 8,6,6},
  {"North American 1927","MEAN FOR NAD27 Central America","NAS-N","CC", 0,125,194, 8,3,5},
  {"North American 1927","Canal Zone","NAS-O", "CC", 0,125,201, 20,20,20},
  {"North American 1927","MEAN FOR NAD27 West Indies","NAS-P", "CC", -3,142,183, 3,9,12},
  {"North American 1927","Bahamas (Except San Salvador Island)","NAS-Q", "CC", -4,154,178, 5,3,5},
  {"North American 1927","Bahamas (San Salvador Island)","NAS-R", "CC", 1,140,165, 25,25,25},
  {"North American 1927","Cuba","NAS-T", "CC", -9,152,178, 25,25,25},
  {"North American 1927","Greenland (Hayes Peninsula)","NAS-U", "CC", 11,114,195, 25,25,25},
  {"North American 1927","Alaska (Aleutian Ids East of 180W)","NAS-V", "CC", -2,152,149, 6,8,10},
  {"North American 1927","Alaska (Aleutian Ids West of 180W)","NAS-W", "CC", 2,204,105, 10,10,10},
  {"North Sahara 1959","Algeria","NSD","CD", -186,-93,310, 25,25,25},
  {"Old Egyptian 1907","Egypt","OEG","HE", -130,110,-13, 3,6,8},
  {"Ordnance Survey Great Britain 1936","England","OGB-A","AA", 371,-112,434, 5,5,6},
  {"Ordnance Survey Great Britain 1936","England, Isle of Man, Wales","OGB-B","AA", 371,-111,434, 10,10,15},
  {"Ordnance Survey Great Britain 1936","Scotland, Shetland Islands","OGB-C","AA", 384,-111,425, 10,10,10},
  {"Ordnance Survey Great Britain 1936","Wales","OGB-D","AA", 370,-108,434, 20,20,20},
  {"Ordnance Survey Great Britain 1936","MEAN FOR OSGB36","OGB-M","AA", 375,-111,431, 10,10,15},
  {"Old Hawaiian","Hawaii","OHA-A", "CC", 89,-279,-183, 25,25,25},
  {"Old Hawaiian","Kauai","OHA-B","CC", 45,-290,-172, 20,20,20},
  {"Old Hawaiian","Maui","OHA-C", "CC", 65,-290,-190, 25,25,25},
  {"Old Hawaiian","Oahu","OHA-D", "CC", 58,-283,-182, 10,6,6},
  {"Old Hawaiian","MEAN FOR Hawaii, Kauai, Maui, Oahu","OHA-M","CC", 61,-285,-181, 25,20,20},
  {"Old Hawaiian Int","Hawaii","OHA-P", "IN", 190,-230,-341, 25,25,25},
  {"Old Hawaiian Int","Maui","OHA-Q", "IN", 210,-230,-357, 25,25,25},
  {"Old Hawaiian Int","Oahu","OHA-R", "IN", 201,-224,-349, 25,25,25},
  {"Ayabelle Lighthouse","Djibouti","PHA", "CD", -79,-129,145, 25,25,25},
  {"Pitcairn Astro 1967","Pitcairn Island","PIT","IN", 185,165,42, 25,25,25},
  {"Pico de las Nieves","Canary Islands","PLN", "IN", -307,-92,127, 25,25,25},
  {"Porto Santo 1936","Porto Santo, Madeira Islands","POS","IN", -499,-249,314, 25,25,25},
  {"Provisional South American 1956","Bolivia","PRP-A","IN", -270,188,-388, 5,11,14},
  {"Provisional South American 1956","Chile (Northern, Near 19�S)","PRP-B", "IN", -270,183,-390, 25,25,25},
  {"Provisional South American 1956","Chile (Southern, Near 43�S)","PRP-C", "IN", -305,243,-442, 20,20,20},
  {"Provisional South American 1956","Colombia","PRP-D", "IN", -282,169,-371, 15,15,15},
  {"Provisional South American 1956","Ecuador","PRP-E", "IN", -278,171,-367, 3,5,3},
  {"Provisional South American 1956","Guyana","PRP-F", "IN", -298,159,-369, 6,14,5},
  {"Provisional South American 1956","Peru","PRP-G", "IN", -279,175,-379, 6,8,12},
  {"Provisional South American 1956","Venezuela","PRP-H", "IN", -295,173,-371, 9,14,15},
  {"Provisional South American 1956","MEAN FOR PSAD56","PRP-M", "IN", -288,175,-376, 17,27,27},
  {"Point 58","MEAN FOR Burkina Faso & Niger","PTB", "CD", -106,-129,165, 25,25,25},
  {"Pointe Noire 1948","Congo","PTN", "CD", -148,51,-291, 25,25,25},
  {"Puerto Rico","Puerto Rico, Virgin Islands","PUR", "CC", 11,72,-101, 3,3,3},
  {"Pulkovo 1942","Russia","PUK", "KA", 28,-130,-95, 0,0,0},
  {"Qatar National","Qatar","QAT", "IN", -128,-283,22, 20,20,20},
  {"Qornoq","Greenland (South)","QUO", "IN", 164,138,-189, 25,25,32},
  {"Reunion","Mascarene Islands","REU", "IN", 94,-948,-1262, 25,25,25},
  {"Santo (DOS) 1965","Espirito Santo Island","SAE", "IN", 170,42,84, 25,25,25},
  {"South American 1969","Argentina","SAN-A", "SA", -62,-1,-37, 5,5,5},
  {"South American 1969","Bolivia","SAN-B", "SA", -61,2,-48, 15,15,15},
  {"South American 1969","Brazil","SAN-C", "SA", -60,-2,-41, 3,5,5},
  {"South American 1969","Chile","SAN-D", "SA", -75,-1,-44, 15,8,11},
  {"South American 1969","Colombia","SAN-E", "SA", -44,6,-36, 6,6,5},
  {"South American 1969","Ecuador","SAN-F", "SA", -48,3,-44, 3,3,3},
  {"South American 1969","Guyana","SAN-G", "SA", -53,3,-47, 9,5,5},
  {"South American 1969","Paraguay","SAN-H", "SA", -61,2,-33, 15,15,15},
  {"South American 1969","Peru [69]","SAN-I", "SA", -58,0,-44, 5,5,5},
  {"South American 1969","Ecuador (Baltra, Galapagos)","SAN-J", "SA", -47,26,-42, 25,25,25},
  {"South American 1969","Trinidad & Tobago","SAN-K", "SA", -45,12,-33, 25,25,25},
  {"South American 1969","Venezuela","SAN-L", "SA", -45,8,-33, 3,6,3},
  {"South American 1969","MEAN FOR SAD69","SAN-M", "SA", -57,1,-41, 15,6,9},
  {"Sao Braz","Azores (Sao Miguel, Santa Maria Islands)","SAO", "IN", -203,141,53, 25,25,25},
  {"Sapper Hill 1943","East Falkland Island","SAP", "IN", -355,21,72, 1,1,1},
  {"Schwarzeck","Namibia","SCK", "BN", 616,97,-251, 20,20,20},
  {"Selvagem Grande 1938","Salvage Islands","SGM", "IN", -289,-124,60, 25,25,25},
  {"Astro DOS 71-4","St Helena Island","SHB", "IN", -320,550,-494, 25,25,25},
  {"Sierra Leone 1960","Sierra Leone","SLE", "CD", -88,4,101, 15,15,15},
  {"South Asia","Singapore","SOA", "FA", 7,-10,-26, 25,25,25},
  {"S-42 (Pulkovo 1942)","Albania","SPK-A", "KA", 24,-130,-92, 3,3,32},
  {"S-42 (Pulkovo 1942)","Czechoslavakia","SPK-C", "KA", 26,-121,-78, 3,3,2},
  {"S-42 (Pulkovo 1942)","Hungary","SPK-H", "KA", 28,-121,-77, 2,2,2},
  {"S-42 (Pulkovo 1942)","Kazakhstan","SPK-K", "KA", 15,-130,-84, 25,25,25},
  {"S-42 (Pulkovo 1942)","Latvia","SPK-L", "KA", 24,-124,-82, 2,2,2},
  {"S-42 (Pulkovo 1942)","Poland","SPK-P", "KA", 23,-124,-82, 4,2,4},
  {"S-42 (Pulkovo 1942)","Romania,","SPK-R", "KA", 28,-121,-77, 3,5,3},
  {"Tananarive Observatory 1925","Madagascar","TAN", "IN", -189,-242,-91, 0,0,0},
  {"Tristan Astro 1968","Tristan da Cunha","TDC", "IN", -632,438,-609, 25,25,25},
  {"Timbalai 1948","Brunei, East Malaysia (Sabah, Sarawak)","TIL", "EB", -679,669,-48, 10,10,12},
  {"Tokyo","Japan","TOY-A", "BR", -148,507,685, 8,5,8},
  {"Tokyo","South Korea","TOY-B", "BR", -147,506,687, 2,2,2},
  {"Tokyo","Okinawa","TOY-C", "BR", -158,507,676, 20,5,20},
  {"Tokyo","MEAN FOR Japan, South Korea, Okinawa","TOY-M", "BR", -148,507,685, 20,5,20},
  {"Astro Tern Island (FRIG) 1961","Tern Island","TRN", "IN", 114,-116,-333, 25,25,25},
  {"Voirol 1960","Algeria","VOR","CD", -123,-206,219, 25,25,25},
  {"WGS 66","Global Definition I","W66","WB", 0,0,0, 0,0,0},
  {"WGS 72","Global Definition I","W72","WD", 0,0,0, 3,3,3},
  {"WGS 84","Global Definition II","WGS84","WE", 0,0,0, 0,0,0},
  {"WGS 84","Global Definition II","W84","WE", 0,0,0, 0,0,0},
  {"Wake Island Astro 1952","Wake Atoll","WAK","IN", 276,-57,149, 25,25,25},
  {"Yacare","Uruguay","YAC","IN", -155,171,37, 0,0,0},
  {"Zanderij","Suriname","ZAN","IN", -265,120,-358, 5,5,8},
  {"CMC Special","Weather Prediction","CMC","CM", 0,0,0, 0,0,0},
  {"END","END","END","", 0,0,0, 0,0,0}
};


#endif

