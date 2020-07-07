/*
 *  OpacityTables.cpp
 *  vipre
 *
 *  Created by Christian Noon on 2/14/12.
 *  Copyright 2012 Christian Noon. All rights reserved.
 *
 */

#include <vipre/Log.hpp>
#include <vipre/OpacityTables.hpp>
#include <vipre/String.hpp>

using namespace vipre;

OpacityTables::OpacityTables() :
    _textures()
{
    ;
}

OpacityTables::~OpacityTables()
{
    ;
}

void OpacityTables::generateAllTables()
{
	createLinearSmoothTable();
	createLinearSharpTable();
	createNormalSmoothTable();
	createNormalSharpTable();
}

void OpacityTables::loadOpacityTable(String opacityTable, osg::ref_ptr<osg::StateSet> stateset, unsigned int textureUnit)
{
    if (_textures.find(opacityTable) == _textures.end())
    {
        vipreLogWARNING(viprePrefix) << "Failed to load the opacity table: " << opacityTable << std::endl;
    }
    else
    {
        vipreLogDEBUG(viprePrefix) << "Setting opacity table: " << opacityTable << std::endl;
        osg::ref_ptr<osg::Texture1D> texture = _textures.find(opacityTable)->second;
        stateset->setTextureAttributeAndModes(textureUnit, texture.get());
    }
}

void OpacityTables::createLinearSmoothTable()
{
	// Create the opacity array
	float* opacity = new float[256];
	for (int i = 0; i < 256; ++i)
		opacity[i] = (float)i / 750.0f;

	// Create a new texture and populate the texture with the opacity array
    osg::ref_ptr<osg::Texture1D> texture = new osg::Texture1D();
    _textures["Linear Smooth"] = texture;
	buildTexture(texture, opacity);
}

void OpacityTables::createLinearSharpTable()
{
	// Create the opacity array
	float* opacity = new float[256];
	for (int i = 0; i < 256; ++i)
		opacity[i] = (float)i / 255.0f;

	// Create a new texture and populate the texture with the opacity array
    osg::ref_ptr<osg::Texture1D> texture = new osg::Texture1D();
    _textures["Linear Sharp"] = texture;
	buildTexture(texture, opacity);
}

void OpacityTables::createNormalSmoothTable()
{
	// Create the opacity array
	float* opacity = new float[256];
	opacity[0] = 0.0;
	opacity[1] = 0.00035196400512637824;
	opacity[2] = 0.0007326924298525271;
	opacity[3] = 0.0011442236976146505;
	opacity[4] = 0.0015887245770282693;
	opacity[5] = 0.00206846753273893;
	opacity[6] = 0.002585868474004356;
	opacity[7] = 0.003143460330686941;
	opacity[8] = 0.0037439157024030437;
	opacity[9] = 0.00439003930880655;
	opacity[10] = 0.005084779314163526;
	opacity[11] = 0.005831234877068647;
	opacity[12] = 0.006632648600728762;
	opacity[13] = 0.007492414082679331;
	opacity[14] = 0.008414075914784418;
	opacity[15] = 0.00940134478266956;
	opacity[16] = 0.010458082366288899;
	opacity[17] = 0.011588316439358044;
	opacity[18] = 0.012796229544779431;
	opacity[19] = 0.014086177868933387;
	opacity[20] = 0.015462676142245294;
	opacity[21] = 0.016930393864327337;
	opacity[22] = 0.01849418172798604;
	opacity[23] = 0.020159030095781894;
	opacity[24] = 0.02193010674861149;
	opacity[25] = 0.023812722911983608;
	opacity[26] = 0.02581235213031028;
	opacity[27] = 0.02793461139261571;
	opacity[28] = 0.030185257357678068;
	opacity[29] = 0.032570193903745916;
	opacity[30] = 0.03509544192967249;
	opacity[31] = 0.03776715822920677;
	opacity[32] = 0.04059160151726953;
	opacity[33] = 0.04357513997966982;
	opacity[34] = 0.04672423617367205;
	opacity[35] = 0.05004542815370492;
	opacity[36] = 0.053545321921645024;
	opacity[37] = 0.05723057632738397;
	opacity[38] = 0.061107895519111936;
	opacity[39] = 0.06518398741987731;
	opacity[40] = 0.06946558637673601;
	opacity[41] = 0.07395939276302001;
	opacity[42] = 0.07867207675319553;
	opacity[43] = 0.08361026322343025;
	opacity[44] = 0.08878048645329467;
	opacity[45] = 0.09418918635090391;
	opacity[46] = 0.09984267070433556;
	opacity[47] = 0.10574710763191322;
	opacity[48] = 0.11190848405876622;
	opacity[49] = 0.11833258306768023;
	opacity[50] = 0.12502496124994797;
	opacity[51] = 0.131990907181929;
	opacity[52] = 0.13923543010047482;
	opacity[53] = 0.14676322592920515;
	opacity[54] = 0.15457862820535104;
	opacity[55] = 0.16268560430489665;
	opacity[56] = 0.17108771391913888;
	opacity[57] = 0.17978806753124713;
	opacity[58] = 0.18878931131683016;
	opacity[59] = 0.1980935931702125;
	opacity[60] = 0.2077025211809937;
	opacity[61] = 0.21761714853461592;
	opacity[62] = 0.22783793198892313;
	opacity[63] = 0.2383647092250121;
	opacity[64] = 0.2491966724232248;
	opacity[65] = 0.26033233051456617;
	opacity[66] = 0.2717694978561297;
	opacity[67] = 0.28350525648251496;
	opacity[68] = 0.2955359447812533;
	opacity[69] = 0.3078571159693671;
	opacity[70] = 0.3204635494179448;
	opacity[71] = 0.33334919780412575;
	opacity[72] = 0.3465071984356746;
	opacity[73] = 0.35992984682697426;
	opacity[74] = 0.373608589149309;
	opacity[75] = 0.38753400713143193;
	opacity[76] = 0.40169582183442315;
	opacity[77] = 0.4160828861019731;
	opacity[78] = 0.43068317323580835;
	opacity[79] = 0.4454838034196984;
	opacity[80] = 0.4604710323948819;
	opacity[81] = 0.47563025523492425;
	opacity[82] = 0.4909460365445836;
	opacity[83] = 0.5064021142346686;
	opacity[84] = 0.5219814183963306;
	opacity[85] = 0.5376660977250697;
	opacity[86] = 0.5534375383950266;
	opacity[87] = 0.5692764055824233;
	opacity[88] = 0.5851626585649955;
	opacity[89] = 0.6010756073448654;
	opacity[90] = 0.6169939277479756;
	opacity[91] = 0.632895725596678;
	opacity[92] = 0.6487585706834575;
	opacity[93] = 0.6645595420692321;
	opacity[94] = 0.6802752884810821;
	opacity[95] = 0.6958820773854087;
	opacity[96] = 0.7113558553856649;
	opacity[97] = 0.7266723010703701;
	opacity[98] = 0.7418068929605582;
	opacity[99] = 0.7567349623577934;
	opacity[100] = 0.771431772616191;
	opacity[101] = 0.7858725795401507;
	opacity[102] = 0.8000326955569451;
	opacity[103] = 0.8138875652138842;
	opacity[104] = 0.827412833125764;
	opacity[105] = 0.8405844119223128;
	opacity[106] = 0.8533785501956411;
	opacity[107] = 0.8657719193219787;
	opacity[108] = 0.8777416700845491;
	opacity[109] = 0.8892654930713005;
	opacity[110] = 0.9003217130463614;
	opacity[111] = 0.9108893417980553;
	opacity[112] = 0.9209481423114904;
	opacity[113] = 0.9304786967160077;
	opacity[114] = 0.9394624742326291;
	opacity[115] = 0.9478818764723552;
	opacity[116] = 0.9557203167081884;
	opacity[117] = 0.9629622462991403;
	opacity[118] = 0.9695932377371126;
	opacity[119] = 0.9755999997463295;
	opacity[120] = 0.9809704565553606;
	opacity[121] = 0.9856937554468372;
	opacity[122] = 0.9897603233803257;
	opacity[123] = 0.9931618896414764;
	opacity[124] = 0.9958915273654647;
	opacity[125] = 0.9979436573118483;
	opacity[126] = 0.9993140856131502;
	opacity[127] = 1.0;
	opacity[128] = 1.0;
	opacity[129] = 0.9993140856131502;
	opacity[130] = 0.9979436573118483;
	opacity[131] = 0.9958915273654647;
	opacity[132] = 0.9931618896414764;
	opacity[133] = 0.9897603233803257;
	opacity[134] = 0.9856937554468372;
	opacity[135] = 0.9809704565553606;
	opacity[136] = 0.9755999997463295;
	opacity[137] = 0.9695932377371126;
	opacity[138] = 0.9629622462991403;
	opacity[139] = 0.9557203167081884;
	opacity[140] = 0.9478818764723552;
	opacity[141] = 0.9394624742326291;
	opacity[142] = 0.9304786967160077;
	opacity[143] = 0.9209481423114904;
	opacity[144] = 0.9108893417980553;
	opacity[145] = 0.9003217130463614;
	opacity[146] = 0.8892654930713005;
	opacity[147] = 0.8777416700845491;
	opacity[148] = 0.8657719193219787;
	opacity[149] = 0.8533785501956411;
	opacity[150] = 0.8405844119223128;
	opacity[151] = 0.827412833125764;
	opacity[152] = 0.8138875652138842;
	opacity[153] = 0.8000326955569451;
	opacity[154] = 0.7858725795401507;
	opacity[155] = 0.771431772616191;
	opacity[156] = 0.7567349623577934;
	opacity[157] = 0.7418068929605582;
	opacity[158] = 0.7266723010703701;
	opacity[159] = 0.7113558553856649;
	opacity[160] = 0.6958820773854087;
	opacity[161] = 0.6802752884810821;
	opacity[162] = 0.6645595420692321;
	opacity[163] = 0.6487585706834575;
	opacity[164] = 0.632895725596678;
	opacity[165] = 0.6169939277479756;
	opacity[166] = 0.6010756073448654;
	opacity[167] = 0.5851626585649955;
	opacity[168] = 0.5692764055824233;
	opacity[169] = 0.5534375383950266;
	opacity[170] = 0.5376660977250697;
	opacity[171] = 0.5219814183963306;
	opacity[172] = 0.5064021142346686;
	opacity[173] = 0.4909460365445836;
	opacity[174] = 0.47563025523492425;
	opacity[175] = 0.4604710323948819;
	opacity[176] = 0.4454838034196984;
	opacity[177] = 0.43068317323580835;
	opacity[178] = 0.4160828861019731;
	opacity[179] = 0.40169582183442315;
	opacity[180] = 0.38753400713143193;
	opacity[181] = 0.373608589149309;
	opacity[182] = 0.35992984682697426;
	opacity[183] = 0.3465071984356746;
	opacity[184] = 0.33334919780412575;
	opacity[185] = 0.3204635494179448;
	opacity[186] = 0.3078571159693671;
	opacity[187] = 0.2955359447812533;
	opacity[188] = 0.28350525648251496;
	opacity[189] = 0.2717694978561297;
	opacity[190] = 0.26033233051456617;
	opacity[191] = 0.2491966724232248;
	opacity[192] = 0.2383647092250121;
	opacity[193] = 0.22783793198892313;
	opacity[194] = 0.21761714853461592;
	opacity[195] = 0.2077025211809937;
	opacity[196] = 0.1980935931702125;
	opacity[197] = 0.18878931131683016;
	opacity[198] = 0.17978806753124713;
	opacity[199] = 0.17108771391913888;
	opacity[200] = 0.16268560430489665;
	opacity[201] = 0.15457862820535104;
	opacity[202] = 0.14676322592920515;
	opacity[203] = 0.13923543010047482;
	opacity[204] = 0.131990907181929;
	opacity[205] = 0.12502496124994797;
	opacity[206] = 0.11833258306768023;
	opacity[207] = 0.11190848405876622;
	opacity[208] = 0.10574710763191322;
	opacity[209] = 0.09984267070433556;
	opacity[210] = 0.09418918635090391;
	opacity[211] = 0.08878048645329467;
	opacity[212] = 0.08361026322343025;
	opacity[213] = 0.07867207675319553;
	opacity[214] = 0.07395939276302001;
	opacity[215] = 0.06946558637673601;
	opacity[216] = 0.06518398741987731;
	opacity[217] = 0.061107895519111936;
	opacity[218] = 0.05723057632738397;
	opacity[219] = 0.053545321921645024;
	opacity[220] = 0.05004542815370492;
	opacity[221] = 0.04672423617367205;
	opacity[222] = 0.04357513997966982;
	opacity[223] = 0.04059160151726953;
	opacity[224] = 0.03776715822920677;
	opacity[225] = 0.03509544192967249;
	opacity[226] = 0.032570193903745916;
	opacity[227] = 0.030185257357678068;
	opacity[228] = 0.02793461139261571;
	opacity[229] = 0.02581235213031028;
	opacity[230] = 0.023812722911983608;
	opacity[231] = 0.02193010674861149;
	opacity[232] = 0.020159030095781894;
	opacity[233] = 0.01849418172798604;
	opacity[234] = 0.016930393864327337;
	opacity[235] = 0.015462676142245294;
	opacity[236] = 0.014086177868933387;
	opacity[237] = 0.012796229544779431;
	opacity[238] = 0.011588316439358044;
	opacity[239] = 0.010458082366288899;
	opacity[240] = 0.00940134478266956;
	opacity[241] = 0.008414075914784418;
	opacity[242] = 0.007492414082679331;
	opacity[243] = 0.006632648600728762;
	opacity[244] = 0.005831234877068647;
	opacity[245] = 0.005084779314163526;
	opacity[246] = 0.00439003930880655;
	opacity[247] = 0.0037439157024030437;
	opacity[248] = 0.003143460330686941;
	opacity[249] = 0.002585868474004356;
	opacity[250] = 0.00206846753273893;
	opacity[251] = 0.0015887245770282693;
	opacity[252] = 0.0011442236976146505;
	opacity[253] = 0.0007326924298525271;
	opacity[254] = 0.00035196400512637824;
	opacity[255] = 0.0;

	for (int i = 0; i < 256; ++i)
		opacity[i] *= 0.4;

	// Create a new texture and populate the texture with the opacity array
    osg::ref_ptr<osg::Texture1D> texture = new osg::Texture1D();
    _textures["Normal Smooth"] = texture;
	buildTexture(texture, opacity);
}

void OpacityTables::createNormalSharpTable()
{
	// Create the opacity array
	float* opacity = new float[256];
	opacity[0] = 0.0;
	opacity[1] = 0.00035196400512637824;
	opacity[2] = 0.0007326924298525271;
	opacity[3] = 0.0011442236976146505;
	opacity[4] = 0.0015887245770282693;
	opacity[5] = 0.00206846753273893;
	opacity[6] = 0.002585868474004356;
	opacity[7] = 0.003143460330686941;
	opacity[8] = 0.0037439157024030437;
	opacity[9] = 0.00439003930880655;
	opacity[10] = 0.005084779314163526;
	opacity[11] = 0.005831234877068647;
	opacity[12] = 0.006632648600728762;
	opacity[13] = 0.007492414082679331;
	opacity[14] = 0.008414075914784418;
	opacity[15] = 0.00940134478266956;
	opacity[16] = 0.010458082366288899;
	opacity[17] = 0.011588316439358044;
	opacity[18] = 0.012796229544779431;
	opacity[19] = 0.014086177868933387;
	opacity[20] = 0.015462676142245294;
	opacity[21] = 0.016930393864327337;
	opacity[22] = 0.01849418172798604;
	opacity[23] = 0.020159030095781894;
	opacity[24] = 0.02193010674861149;
	opacity[25] = 0.023812722911983608;
	opacity[26] = 0.02581235213031028;
	opacity[27] = 0.02793461139261571;
	opacity[28] = 0.030185257357678068;
	opacity[29] = 0.032570193903745916;
	opacity[30] = 0.03509544192967249;
	opacity[31] = 0.03776715822920677;
	opacity[32] = 0.04059160151726953;
	opacity[33] = 0.04357513997966982;
	opacity[34] = 0.04672423617367205;
	opacity[35] = 0.05004542815370492;
	opacity[36] = 0.053545321921645024;
	opacity[37] = 0.05723057632738397;
	opacity[38] = 0.061107895519111936;
	opacity[39] = 0.06518398741987731;
	opacity[40] = 0.06946558637673601;
	opacity[41] = 0.07395939276302001;
	opacity[42] = 0.07867207675319553;
	opacity[43] = 0.08361026322343025;
	opacity[44] = 0.08878048645329467;
	opacity[45] = 0.09418918635090391;
	opacity[46] = 0.09984267070433556;
	opacity[47] = 0.10574710763191322;
	opacity[48] = 0.11190848405876622;
	opacity[49] = 0.11833258306768023;
	opacity[50] = 0.12502496124994797;
	opacity[51] = 0.131990907181929;
	opacity[52] = 0.13923543010047482;
	opacity[53] = 0.14676322592920515;
	opacity[54] = 0.15457862820535104;
	opacity[55] = 0.16268560430489665;
	opacity[56] = 0.17108771391913888;
	opacity[57] = 0.17978806753124713;
	opacity[58] = 0.18878931131683016;
	opacity[59] = 0.1980935931702125;
	opacity[60] = 0.2077025211809937;
	opacity[61] = 0.21761714853461592;
	opacity[62] = 0.22783793198892313;
	opacity[63] = 0.2383647092250121;
	opacity[64] = 0.2491966724232248;
	opacity[65] = 0.26033233051456617;
	opacity[66] = 0.2717694978561297;
	opacity[67] = 0.28350525648251496;
	opacity[68] = 0.2955359447812533;
	opacity[69] = 0.3078571159693671;
	opacity[70] = 0.3204635494179448;
	opacity[71] = 0.33334919780412575;
	opacity[72] = 0.3465071984356746;
	opacity[73] = 0.35992984682697426;
	opacity[74] = 0.373608589149309;
	opacity[75] = 0.38753400713143193;
	opacity[76] = 0.40169582183442315;
	opacity[77] = 0.4160828861019731;
	opacity[78] = 0.43068317323580835;
	opacity[79] = 0.4454838034196984;
	opacity[80] = 0.4604710323948819;
	opacity[81] = 0.47563025523492425;
	opacity[82] = 0.4909460365445836;
	opacity[83] = 0.5064021142346686;
	opacity[84] = 0.5219814183963306;
	opacity[85] = 0.5376660977250697;
	opacity[86] = 0.5534375383950266;
	opacity[87] = 0.5692764055824233;
	opacity[88] = 0.5851626585649955;
	opacity[89] = 0.6010756073448654;
	opacity[90] = 0.6169939277479756;
	opacity[91] = 0.632895725596678;
	opacity[92] = 0.6487585706834575;
	opacity[93] = 0.6645595420692321;
	opacity[94] = 0.6802752884810821;
	opacity[95] = 0.6958820773854087;
	opacity[96] = 0.7113558553856649;
	opacity[97] = 0.7266723010703701;
	opacity[98] = 0.7418068929605582;
	opacity[99] = 0.7567349623577934;
	opacity[100] = 0.771431772616191;
	opacity[101] = 0.7858725795401507;
	opacity[102] = 0.8000326955569451;
	opacity[103] = 0.8138875652138842;
	opacity[104] = 0.827412833125764;
	opacity[105] = 0.8405844119223128;
	opacity[106] = 0.8533785501956411;
	opacity[107] = 0.8657719193219787;
	opacity[108] = 0.8777416700845491;
	opacity[109] = 0.8892654930713005;
	opacity[110] = 0.9003217130463614;
	opacity[111] = 0.9108893417980553;
	opacity[112] = 0.9209481423114904;
	opacity[113] = 0.9304786967160077;
	opacity[114] = 0.9394624742326291;
	opacity[115] = 0.9478818764723552;
	opacity[116] = 0.9557203167081884;
	opacity[117] = 0.9629622462991403;
	opacity[118] = 0.9695932377371126;
	opacity[119] = 0.9755999997463295;
	opacity[120] = 0.9809704565553606;
	opacity[121] = 0.9856937554468372;
	opacity[122] = 0.9897603233803257;
	opacity[123] = 0.9931618896414764;
	opacity[124] = 0.9958915273654647;
	opacity[125] = 0.9979436573118483;
	opacity[126] = 0.9993140856131502;
	opacity[127] = 1.0;
	opacity[128] = 1.0;
	opacity[129] = 0.9993140856131502;
	opacity[130] = 0.9979436573118483;
	opacity[131] = 0.9958915273654647;
	opacity[132] = 0.9931618896414764;
	opacity[133] = 0.9897603233803257;
	opacity[134] = 0.9856937554468372;
	opacity[135] = 0.9809704565553606;
	opacity[136] = 0.9755999997463295;
	opacity[137] = 0.9695932377371126;
	opacity[138] = 0.9629622462991403;
	opacity[139] = 0.9557203167081884;
	opacity[140] = 0.9478818764723552;
	opacity[141] = 0.9394624742326291;
	opacity[142] = 0.9304786967160077;
	opacity[143] = 0.9209481423114904;
	opacity[144] = 0.9108893417980553;
	opacity[145] = 0.9003217130463614;
	opacity[146] = 0.8892654930713005;
	opacity[147] = 0.8777416700845491;
	opacity[148] = 0.8657719193219787;
	opacity[149] = 0.8533785501956411;
	opacity[150] = 0.8405844119223128;
	opacity[151] = 0.827412833125764;
	opacity[152] = 0.8138875652138842;
	opacity[153] = 0.8000326955569451;
	opacity[154] = 0.7858725795401507;
	opacity[155] = 0.771431772616191;
	opacity[156] = 0.7567349623577934;
	opacity[157] = 0.7418068929605582;
	opacity[158] = 0.7266723010703701;
	opacity[159] = 0.7113558553856649;
	opacity[160] = 0.6958820773854087;
	opacity[161] = 0.6802752884810821;
	opacity[162] = 0.6645595420692321;
	opacity[163] = 0.6487585706834575;
	opacity[164] = 0.632895725596678;
	opacity[165] = 0.6169939277479756;
	opacity[166] = 0.6010756073448654;
	opacity[167] = 0.5851626585649955;
	opacity[168] = 0.5692764055824233;
	opacity[169] = 0.5534375383950266;
	opacity[170] = 0.5376660977250697;
	opacity[171] = 0.5219814183963306;
	opacity[172] = 0.5064021142346686;
	opacity[173] = 0.4909460365445836;
	opacity[174] = 0.47563025523492425;
	opacity[175] = 0.4604710323948819;
	opacity[176] = 0.4454838034196984;
	opacity[177] = 0.43068317323580835;
	opacity[178] = 0.4160828861019731;
	opacity[179] = 0.40169582183442315;
	opacity[180] = 0.38753400713143193;
	opacity[181] = 0.373608589149309;
	opacity[182] = 0.35992984682697426;
	opacity[183] = 0.3465071984356746;
	opacity[184] = 0.33334919780412575;
	opacity[185] = 0.3204635494179448;
	opacity[186] = 0.3078571159693671;
	opacity[187] = 0.2955359447812533;
	opacity[188] = 0.28350525648251496;
	opacity[189] = 0.2717694978561297;
	opacity[190] = 0.26033233051456617;
	opacity[191] = 0.2491966724232248;
	opacity[192] = 0.2383647092250121;
	opacity[193] = 0.22783793198892313;
	opacity[194] = 0.21761714853461592;
	opacity[195] = 0.2077025211809937;
	opacity[196] = 0.1980935931702125;
	opacity[197] = 0.18878931131683016;
	opacity[198] = 0.17978806753124713;
	opacity[199] = 0.17108771391913888;
	opacity[200] = 0.16268560430489665;
	opacity[201] = 0.15457862820535104;
	opacity[202] = 0.14676322592920515;
	opacity[203] = 0.13923543010047482;
	opacity[204] = 0.131990907181929;
	opacity[205] = 0.12502496124994797;
	opacity[206] = 0.11833258306768023;
	opacity[207] = 0.11190848405876622;
	opacity[208] = 0.10574710763191322;
	opacity[209] = 0.09984267070433556;
	opacity[210] = 0.09418918635090391;
	opacity[211] = 0.08878048645329467;
	opacity[212] = 0.08361026322343025;
	opacity[213] = 0.07867207675319553;
	opacity[214] = 0.07395939276302001;
	opacity[215] = 0.06946558637673601;
	opacity[216] = 0.06518398741987731;
	opacity[217] = 0.061107895519111936;
	opacity[218] = 0.05723057632738397;
	opacity[219] = 0.053545321921645024;
	opacity[220] = 0.05004542815370492;
	opacity[221] = 0.04672423617367205;
	opacity[222] = 0.04357513997966982;
	opacity[223] = 0.04059160151726953;
	opacity[224] = 0.03776715822920677;
	opacity[225] = 0.03509544192967249;
	opacity[226] = 0.032570193903745916;
	opacity[227] = 0.030185257357678068;
	opacity[228] = 0.02793461139261571;
	opacity[229] = 0.02581235213031028;
	opacity[230] = 0.023812722911983608;
	opacity[231] = 0.02193010674861149;
	opacity[232] = 0.020159030095781894;
	opacity[233] = 0.01849418172798604;
	opacity[234] = 0.016930393864327337;
	opacity[235] = 0.015462676142245294;
	opacity[236] = 0.014086177868933387;
	opacity[237] = 0.012796229544779431;
	opacity[238] = 0.011588316439358044;
	opacity[239] = 0.010458082366288899;
	opacity[240] = 0.00940134478266956;
	opacity[241] = 0.008414075914784418;
	opacity[242] = 0.007492414082679331;
	opacity[243] = 0.006632648600728762;
	opacity[244] = 0.005831234877068647;
	opacity[245] = 0.005084779314163526;
	opacity[246] = 0.00439003930880655;
	opacity[247] = 0.0037439157024030437;
	opacity[248] = 0.003143460330686941;
	opacity[249] = 0.002585868474004356;
	opacity[250] = 0.00206846753273893;
	opacity[251] = 0.0015887245770282693;
	opacity[252] = 0.0011442236976146505;
	opacity[253] = 0.0007326924298525271;
	opacity[254] = 0.00035196400512637824;
	opacity[255] = 0.0;

	// Create a new texture and populate the texture with the opacity array
    osg::ref_ptr<osg::Texture1D> texture = new osg::Texture1D();
    _textures["Normal Sharp"] = texture;
	buildTexture(texture, opacity);
}

void OpacityTables::buildTexture(osg::ref_ptr<osg::Texture1D> texture, float* opacity)
{
	// Create a new opacity table image out of the new opacity table array
	osg::ref_ptr<osg::Image> image = new osg::Image;
	image->setImage(256, 1, 1, GL_LUMINANCE, GL_LUMINANCE, GL_FLOAT,
					(unsigned char*)opacity, osg::Image::USE_NEW_DELETE);

	// Create the opacity table texture
	texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	texture->setImage(image.get());
}
