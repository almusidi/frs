// ------------------------------------------------------------
// -----                  R3BFrsOnlineSpectra             -----
// -----    Created 29/07/18  by J.L. Rodriguez-Sanchez   -----
// -----           Fill FRS online histograms             -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with FRS online data
 */

#include "R3BFrsOnlineSpectra.h"
#include "R3BEventHeader.h"
#include "R3BFrsData.h"
#include "R3BFrsMappedData.h"
#include "R3BMusicCalData.h"
#include "R3BMusicHitData.h"
#include "R3BMusicMappedData.h"
#include "R3BMwHitData.h"
#include "R3BSeetramCalData.h"
#include "R3BTpcCalData.h"
#include "R3BTpcHitData.h"
#include "R3BWRMasterData.h"
#include "THttpServer.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "TCanvas.h"
#include "TFolder.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TVector3.h"

#include "TClonesArray.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TMath.h"
#include "TRandom.h"
#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <fstream>
#include <iostream>
#include <sstream>

double tpcpos[5] = { 604., 1782.5, 390., 1530., 2300. }; // FIXME

using namespace std;

R3BFrsOnlineSpectra::R3BFrsOnlineSpectra()
    : FairTask("FrsOnlineSpectra", 1)
    , fMappedItemsFrs(NULL)
    , fCalItemsSeetram(NULL)
    , fMapItemsMusic(NULL)
    , fCalItemsMusic(NULL)
    , fHitItemsMusic(NULL)
    , fHitItemsTpc(NULL)
    , fCalItemsTpc(NULL)
    , fHitItemsMw(NULL)
    , fAnaItemsFrs(NULL)
    , fWRItemsMaster(NULL)
    , fTrigger(-1)
    , fFirstWr(kTRUE)
    , fFirstValueWR(1.)
    , fWrrange(100)
    , fseetram_range(600)
    , fseetram_rangeC(60)
    , fOffsetSeetram(0)
    , fOffsetSeetramC(0)
    , fNEvents(0)
{
}

R3BFrsOnlineSpectra::R3BFrsOnlineSpectra(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fMappedItemsFrs(NULL)
    , fCalItemsSeetram(NULL)
    , fMapItemsMusic(NULL)
    , fCalItemsMusic(NULL)
    , fHitItemsMusic(NULL)
    , fHitItemsTpc(NULL)
    , fCalItemsTpc(NULL)
    , fHitItemsMw(NULL)
    , fAnaItemsFrs(NULL)
    , fWRItemsMaster(NULL)
    , fTrigger(-1)
    , fFirstWr(kTRUE)
    , fFirstValueWR(1.)
    , fWrrange(100)
    , fseetram_range(600)
    , fseetram_rangeC(60)
    , fOffsetSeetram(0)
    , fOffsetSeetramC(0)
    , fNEvents(0)
{
}

R3BFrsOnlineSpectra::~R3BFrsOnlineSpectra()
{
    LOG(INFO) << "R3BFrsOnlineSpectra::Delete instance";
    if (fMappedItemsFrs)
        delete fMappedItemsFrs;
    if (fCalItemsSeetram)
        delete fCalItemsSeetram;
    if (fHitItemsMusic)
        delete fHitItemsMusic;
    if (fCalItemsMusic)
        delete fCalItemsMusic;
    if (fMapItemsMusic)
        delete fMapItemsMusic;
    if (fHitItemsTpc)
        delete fHitItemsTpc;
    if (fCalItemsTpc)
        delete fCalItemsTpc;
    if (fHitItemsMw)
        delete fHitItemsMw;
    if (fAnaItemsFrs)
        delete fAnaItemsFrs;
    if (fWRItemsMaster)
        delete fWRItemsMaster;
}

InitStatus R3BFrsOnlineSpectra::Init()
{

    LOG(INFO) << "R3BFrsOnlineSpectra::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(FATAL) << "R3BFrsOnlineSpectra::Init FairRootManager not found";
    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");

    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    // get access to Mapped data
    fMappedItemsFrs = (TClonesArray*)mgr->GetObject("FrsMappedData");
    if (!fMappedItemsFrs)
    {
        return kFATAL;
    }

    // get access to WR-Master data
    fWRItemsMaster = (TClonesArray*)mgr->GetObject("WRMasterData");
    if (!fWRItemsMaster)
    {
        LOG(INFO) << "R3BCalifaOnlineSpectra::Init WRMasterData not found";
    }

    // get access to Cal Seetram data
    fCalItemsSeetram = (TClonesArray*)mgr->GetObject("SeetramCalData");

    // get access to Map Music data
    fMapItemsMusic = (TClonesArray*)mgr->GetObject("MusicMappedData");

    // get access to Cal Music data
    fCalItemsMusic = (TClonesArray*)mgr->GetObject("MusicCalData");

    // get access to Hit Music data
    fHitItemsMusic = (TClonesArray*)mgr->GetObject("MusicHitData");

    // get access to Cal Tpc data
    fCalItemsTpc = (TClonesArray*)mgr->GetObject("TpcCalData");

    // get access to Hit Tpc data
    fHitItemsTpc = (TClonesArray*)mgr->GetObject("TpcHitData");

    // get access to Hit MW data
    fHitItemsMw = (TClonesArray*)mgr->GetObject("MwHitData");

    // get access to Analysis Frs data
    fAnaItemsFrs = (TClonesArray*)mgr->GetObject("FrsData");

    // create histograms of all detectors

    char Name1[255];
    char Name2[255];

    //  CANVAS 1  -------------------------------
    cMus1 = new TCanvas("Music41_E_raw", "Music41 info", 10, 10, 800, 700);
    cMus1->Divide(4, 2);
    cMus2 = new TCanvas("Music42_E_raw", "Music42 info", 10, 10, 800, 700);
    cMus2->Divide(4, 2);
    cMus3 = new TCanvas("Music43_E_raw", "Music43 info", 10, 10, 800, 700);
    cMus3->Divide(4, 2);

    // Music: Map data
    for (Int_t i = 0; i < 3; i++)
    { // number of Musics
        for (Int_t j = 0; j < 8; j++)
        { // 8 histo per detector
            sprintf(Name1, "fh_music_energy_map_%d", i * 8 + j);
            if (i == 0)
                sprintf(Name2, "Music41 anode %d", j + 1);
            else if (i == 1)
                sprintf(Name2, "Music42 anode %d", j + 1);
            else
                sprintf(Name2, "Music43 anode %d", j + 1);
            fh_music_energy_per_anode[i * 8 + j] = new TH1F(Name1, Name2, 500, 0, 4092);
            fh_music_energy_per_anode[i * 8 + j]->GetXaxis()->SetTitle("Channels");
            fh_music_energy_per_anode[i * 8 + j]->GetYaxis()->SetTitle("Counts");
            fh_music_energy_per_anode[i * 8 + j]->GetYaxis()->SetTitleOffset(1.2);
            fh_music_energy_per_anode[i * 8 + j]->GetXaxis()->CenterTitle(true);
            fh_music_energy_per_anode[i * 8 + j]->GetYaxis()->CenterTitle(true);
            fh_music_energy_per_anode[i * 8 + j]->GetXaxis()->SetLabelSize(0.045);
            fh_music_energy_per_anode[i * 8 + j]->GetXaxis()->SetTitleSize(0.045);
            if (i == 0)
                cMus1->cd(j + 1);
            else if (i == 1)
                cMus2->cd(j + 1);
            else
                cMus3->cd(j + 1);
            fh_music_energy_per_anode[i * 8 + j]->SetFillColor(kGreen - 3);
            fh_music_energy_per_anode[i * 8 + j]->SetLineColor(1);
            fh_music_energy_per_anode[i * 8 + j]->Draw("");
        }
    }

    cMus1cal = new TCanvas("Music41_E_cal", "Music41 calibr. info", 10, 10, 800, 700);
    cMus1cal->Divide(4, 2);
    cMus2cal = new TCanvas("Music42_E_cal", "Music42 calibr. info", 10, 10, 800, 700);
    cMus2cal->Divide(4, 2);
    cMus3cal = new TCanvas("Music43_E_cal", "Music43 calibr. info", 10, 10, 800, 700);
    cMus3cal->Divide(4, 2);

    // Music: Cal data
    for (Int_t i = 0; i < 3; i++)
    { // number of Musics
        for (Int_t j = 0; j < 8; j++)
        { // 8 histo per detector
            sprintf(Name1, "fh_music_energy_cal_%d", i * 8 + j);
            if (i == 0)
                sprintf(Name2, "Music41 anode %d", j + 1);
            else if (i == 1)
                sprintf(Name2, "Music42 anode %d", j + 1);
            else
                sprintf(Name2, "Music43 anode %d", j + 1);
            fh_music_energy_per_anodecal[i * 8 + j] = new TH1F(Name1, Name2, 1000, 0, 4092);
            fh_music_energy_per_anodecal[i * 8 + j]->GetXaxis()->SetTitle("Channels");
            fh_music_energy_per_anodecal[i * 8 + j]->GetYaxis()->SetTitle("Counts");
            fh_music_energy_per_anodecal[i * 8 + j]->GetYaxis()->SetTitleOffset(1.2);
            fh_music_energy_per_anodecal[i * 8 + j]->GetXaxis()->CenterTitle(true);
            fh_music_energy_per_anodecal[i * 8 + j]->GetYaxis()->CenterTitle(true);
            fh_music_energy_per_anodecal[i * 8 + j]->GetXaxis()->SetLabelSize(0.045);
            fh_music_energy_per_anodecal[i * 8 + j]->GetXaxis()->SetTitleSize(0.045);
            if (i == 0)
                cMus1cal->cd(j + 1);
            else if (i == 1)
                cMus2cal->cd(j + 1);
            else
                cMus3cal->cd(j + 1);
            fh_music_energy_per_anodecal[i * 8 + j]->SetFillColor(kGreen - 3);
            fh_music_energy_per_anodecal[i * 8 + j]->SetLineColor(1);
            fh_music_energy_per_anodecal[i * 8 + j]->Draw("");
        }
    }

    cMushit[0] = new TCanvas("Music41_Z", "Music41 hit info", 10, 10, 800, 700);
    sprintf(Name1, "fh_music_Z_%d", 0);
    sprintf(Name2, "Music41 Z");
    fh_music_Z[0] = new TH1F(Name1, Name2, 2000, 0, 60);
    fh_music_Z[0]->GetXaxis()->SetTitle("Charge-Z");
    fh_music_Z[0]->GetYaxis()->SetTitle("Counts");
    fh_music_Z[0]->GetYaxis()->SetTitleOffset(1.2);
    fh_music_Z[0]->GetXaxis()->CenterTitle(true);
    fh_music_Z[0]->GetYaxis()->CenterTitle(true);
    fh_music_Z[0]->GetXaxis()->SetLabelSize(0.045);
    fh_music_Z[0]->GetXaxis()->SetTitleSize(0.045);
    fh_music_Z[0]->SetFillColor(kGreen - 3);
    fh_music_Z[0]->SetLineColor(1);
    fh_music_Z[0]->Draw("");

    cMushit[1] = new TCanvas("Music42_Z", "Music42 hit info", 10, 10, 800, 700);
    sprintf(Name1, "fh_music_Z_%d", 1);
    sprintf(Name2, "Music42 Z");
    fh_music_Z[1] = new TH1F(Name1, Name2, 2000, 0, 60);
    fh_music_Z[1]->GetXaxis()->SetTitle("Charge-Z");
    fh_music_Z[1]->GetYaxis()->SetTitle("Counts");
    fh_music_Z[1]->GetYaxis()->SetTitleOffset(1.2);
    fh_music_Z[1]->GetXaxis()->CenterTitle(true);
    fh_music_Z[1]->GetYaxis()->CenterTitle(true);
    fh_music_Z[1]->GetXaxis()->SetLabelSize(0.045);
    fh_music_Z[1]->GetXaxis()->SetTitleSize(0.045);
    fh_music_Z[1]->SetFillColor(kGreen - 3);
    fh_music_Z[1]->SetLineColor(1);
    fh_music_Z[1]->Draw("");

    cMushit[2] = new TCanvas("Music43_Z", "Music43 hit info", 10, 10, 800, 700);
    sprintf(Name1, "fh_music_Z_%d", 2);
    sprintf(Name2, "Music43 Z");
    fh_music_Z[2] = new TH1F(Name1, Name2, 2000, 0, 60);
    fh_music_Z[2]->GetXaxis()->SetTitle("Charge-Z");
    fh_music_Z[2]->GetYaxis()->SetTitle("Counts");
    fh_music_Z[2]->GetYaxis()->SetTitleOffset(1.2);
    fh_music_Z[2]->GetXaxis()->CenterTitle(true);
    fh_music_Z[2]->GetYaxis()->CenterTitle(true);
    fh_music_Z[2]->GetXaxis()->SetLabelSize(0.045);
    fh_music_Z[2]->GetXaxis()->SetTitleSize(0.045);
    fh_music_Z[2]->SetFillColor(kGreen - 3);
    fh_music_Z[2]->SetLineColor(1);
    fh_music_Z[2]->Draw("");

    cMu41_mu42 = new TCanvas("Music41_vs_Music43", "Music41 vs Music43 info", 10, 10, 800, 700);
    sprintf(Name1, "fh_mu41_vs_mu43");
    sprintf(Name2, "Energy Music41 vs Music43");
    fh_mu41_mu42 = new TH2F(Name1, Name2, 1000, 0, 4092, 1000, 0, 4092);
    fh_mu41_mu42->GetXaxis()->SetTitle("Music41 [Ch]");
    fh_mu41_mu42->GetYaxis()->SetTitle("Music43 [Ch]");
    fh_mu41_mu42->GetXaxis()->SetTitleOffset(1.15);
    fh_mu41_mu42->GetYaxis()->SetTitleOffset(1.15);
    fh_mu41_mu42->GetXaxis()->CenterTitle(true);
    fh_mu41_mu42->GetYaxis()->CenterTitle(true);
    fh_mu41_mu42->GetXaxis()->SetLabelSize(0.045);
    fh_mu41_mu42->GetXaxis()->SetTitleSize(0.045);
    fh_mu41_mu42->Draw("col");

    //  CANVAS 2  -------------------------------
    cTpcCal[0] = new TCanvas("TPC21_csum", "CSum info for Tpcs", 10, 10, 800, 700);
    cTpcCal[0]->Divide(2, 2);
    cTpcCal[1] = new TCanvas("TPC22_csum", "CSum info for Tpcs", 10, 10, 800, 700);
    cTpcCal[1]->Divide(2, 2);
    cTpcCal[2] = new TCanvas("TPC41_csum", "CSum info for Tpcs", 10, 10, 800, 700);
    cTpcCal[2]->Divide(2, 2);
    cTpcCal[3] = new TCanvas("TPC42_csum", "CSum info for Tpcs", 10, 10, 800, 700);
    cTpcCal[3]->Divide(2, 2);
    cTpcCal[4] = new TCanvas("TPC31_csum", "CSum info for Tpcs", 10, 10, 800, 700);
    cTpcCal[4]->Divide(2, 2);
    cCalx = new TCanvas("TPC_DeltaX", "DeltaX info for Tpcs", 10, 10, 800, 700);
    cCalx->Divide(2, 2);
    cCalx31 = new TCanvas("TPC31_Summary", "Info for Tpc 31", 10, 10, 800, 700);
    cCalx31->Divide(2, 2);

    // Csum tpc data
    for (Int_t i = 0; i < 4; i++)
    { // number of TPCs
        sprintf(Name1, "fh_tpc_deltax_%d", i + 1);
        if (i < 2)
            sprintf(Name2, "DeltaX for TPC 2%d at S2", i + 1);
        else
            sprintf(Name2, "DeltaX for TPC 4%d at S4", i - 1);
        fh_tpc_deltax[i] = new TH1F(Name1, Name2, 150, -500, 500);
        fh_tpc_deltax[i]->GetXaxis()->SetTitle("Channels");
        fh_tpc_deltax[i]->GetYaxis()->SetTitle("Counts");
        fh_tpc_deltax[i]->GetYaxis()->SetTitleOffset(1.2);
        fh_tpc_deltax[i]->GetXaxis()->CenterTitle(true);
        fh_tpc_deltax[i]->GetYaxis()->CenterTitle(true);
        fh_tpc_deltax[i]->GetXaxis()->SetLabelSize(0.045);
        fh_tpc_deltax[i]->GetXaxis()->SetTitleSize(0.045);
        cCalx->cd(i + 1);
        fh_tpc_deltax[i]->SetFillColor(kGreen - 3);
        fh_tpc_deltax[i]->SetLineColor(1);
        fh_tpc_deltax[i]->Draw("");
        for (Int_t j = 0; j < 4; j++)
        { // 4 histo per detector
            sprintf(Name1, "fh_tpc_csum_%d", i * 4 + j);
            if (i < 2)
                sprintf(Name2, "CSum for TPC 2%d and DT- %d", i + 1, j);
            else
                sprintf(Name2, "CSum for TPC 4%d and DT- %d", i - 1, j);
            fh_tpc_csum[i * 4 + j] = new TH1F(Name1, Name2, 500, 0, 4092);
            fh_tpc_csum[i * 4 + j]->GetXaxis()->SetTitle("Channels");
            fh_tpc_csum[i * 4 + j]->GetYaxis()->SetTitle("Counts");
            fh_tpc_csum[i * 4 + j]->GetYaxis()->SetTitleOffset(1.2);
            fh_tpc_csum[i * 4 + j]->GetXaxis()->CenterTitle(true);
            fh_tpc_csum[i * 4 + j]->GetYaxis()->CenterTitle(true);
            fh_tpc_csum[i * 4 + j]->GetXaxis()->SetLabelSize(0.045);
            fh_tpc_csum[i * 4 + j]->GetXaxis()->SetTitleSize(0.045);
            cTpcCal[i]->cd(j + 1);
            fh_tpc_csum[i * 4 + j]->SetFillColor(kGreen - 3);
            fh_tpc_csum[i * 4 + j]->SetLineColor(1);
            fh_tpc_csum[i * 4 + j]->Draw("");
        }
    }

    // TPC 31
    sprintf(Name1, "fh_tpc_deltax_31");
    sprintf(Name2, "DeltaX for TPC 31 at S4");
    fh_tpc_deltax[4] = new TH1F(Name1, Name2, 150, -500, 500);
    fh_tpc_deltax[4]->GetXaxis()->SetTitle("Channels");
    fh_tpc_deltax[4]->GetYaxis()->SetTitle("Counts");
    fh_tpc_deltax[4]->GetYaxis()->SetTitleOffset(1.2);
    fh_tpc_deltax[4]->GetXaxis()->CenterTitle(true);
    fh_tpc_deltax[4]->GetYaxis()->CenterTitle(true);
    fh_tpc_deltax[4]->GetXaxis()->SetLabelSize(0.045);
    fh_tpc_deltax[4]->GetXaxis()->SetTitleSize(0.045);
    fh_tpc_deltax[4]->GetYaxis()->SetLabelSize(0.045);
    fh_tpc_deltax[4]->GetYaxis()->SetTitleSize(0.045);
    cCalx31->cd(1);
    fh_tpc_deltax[4]->SetFillColor(kGreen - 3);
    fh_tpc_deltax[4]->SetLineColor(1);
    fh_tpc_deltax[4]->Draw("");
    for (Int_t j = 0; j < 4; j++)
    { // 4 histo per detector
        sprintf(Name1, "fh_tpc_csum_%d", 4 * 4 + j);
        sprintf(Name2, "CSum for TPC 31 and DT- %d", j);
        fh_tpc_csum[16 + j] = new TH1F(Name1, Name2, 500, 0, 4092);
        fh_tpc_csum[16 + j]->GetXaxis()->SetTitle("Channels");
        fh_tpc_csum[16 + j]->GetYaxis()->SetTitle("Counts");
        fh_tpc_csum[16 + j]->GetYaxis()->SetTitleOffset(1.2);
        fh_tpc_csum[16 + j]->GetXaxis()->CenterTitle(true);
        fh_tpc_csum[16 + j]->GetYaxis()->CenterTitle(true);
        fh_tpc_csum[16 + j]->GetXaxis()->SetLabelSize(0.045);
        fh_tpc_csum[16 + j]->GetXaxis()->SetTitleSize(0.045);
        fh_tpc_csum[16 + j]->GetYaxis()->SetLabelSize(0.045);
        fh_tpc_csum[16 + j]->GetYaxis()->SetTitleSize(0.045);
        cTpcCal[4]->cd(j + 1);
        fh_tpc_csum[16 + j]->SetFillColor(kGreen - 3);
        fh_tpc_csum[16 + j]->SetLineColor(1);
        fh_tpc_csum[16 + j]->Draw("");
    }

    //  CANVAS 3  -------------------------------
    cHitx = new TCanvas("TPC_position_X", "TPC Hit info", 10, 10, 800, 700);
    cHitx->Divide(2, 2);

    // Hit TPC data
    for (Int_t i = 0; i < 5; i++)
    { // one histo per detector
        sprintf(Name1, "fh_Tpc_hitx_%d", i + 1);
        if (i < 2)
            sprintf(Name2, "X for TPC 2%d at S2", i + 1);
        else if (i < 4)
            sprintf(Name2, "X for TPC 4%d at S4", i - 1);
        else
            sprintf(Name2, "X for TPC 31 at S4");
        fh_Tpc_hitx[i] = new TH1F(Name1, Name2, 500, -100, 100.);
        fh_Tpc_hitx[i]->GetXaxis()->SetTitle("X [mm]");
        fh_Tpc_hitx[i]->GetYaxis()->SetTitle("Counts");
        fh_Tpc_hitx[i]->GetXaxis()->CenterTitle(true);
        fh_Tpc_hitx[i]->GetYaxis()->CenterTitle(true);
        fh_Tpc_hitx[i]->GetYaxis()->SetTitleOffset(1.15);
        fh_Tpc_hitx[i]->GetXaxis()->SetTitleOffset(1.1);
        fh_Tpc_hitx[i]->GetXaxis()->SetLabelSize(0.045);
        fh_Tpc_hitx[i]->GetXaxis()->SetTitleSize(0.045);
        fh_Tpc_hitx[i]->GetYaxis()->SetLabelSize(0.045);
        fh_Tpc_hitx[i]->GetYaxis()->SetTitleSize(0.045);
        fh_Tpc_hitx[i]->SetFillColor(2);
        fh_Tpc_hitx[i]->SetLineColor(1);
        if (i < 4)
            cHitx->cd(i + 1);
        else
            cCalx31->cd(2);
        fh_Tpc_hitx[i]->Draw("");
    }

    //  CANVAS 4  -------------------------------
    cHity = new TCanvas("TPC_position_Y", "TPC Hit info", 10, 10, 800, 700);
    cHity->Divide(2, 2);

    // Hit TPC data
    for (Int_t i = 0; i < 5; i++)
    { // one histo per detector
        sprintf(Name1, "fh_Tpc_hity_%d", i + 1);
        if (i < 2)
            sprintf(Name2, "Y for TPC 2%d at S2", i + 1);
        else if (i < 4)
            sprintf(Name2, "Y for TPC 4%d at S4", i - 1);
        else
            sprintf(Name2, "Y for TPC 31 at S4");
        fh_Tpc_hity[i] = new TH1F(Name1, Name2, 500, -100, 100.);
        fh_Tpc_hity[i]->GetXaxis()->SetTitle("Y [mm]");
        fh_Tpc_hity[i]->GetYaxis()->SetTitle("Counts");
        fh_Tpc_hity[i]->GetXaxis()->CenterTitle(true);
        fh_Tpc_hity[i]->GetYaxis()->CenterTitle(true);
        fh_Tpc_hity[i]->GetYaxis()->SetTitleOffset(1.15);
        fh_Tpc_hity[i]->GetXaxis()->SetTitleOffset(1.1);
        fh_Tpc_hity[i]->GetXaxis()->SetLabelSize(0.045);
        fh_Tpc_hity[i]->GetXaxis()->SetTitleSize(0.045);
        fh_Tpc_hity[i]->GetYaxis()->SetLabelSize(0.045);
        fh_Tpc_hity[i]->GetYaxis()->SetTitleSize(0.045);
        fh_Tpc_hity[i]->SetFillColor(2);
        fh_Tpc_hity[i]->SetLineColor(1);
        if (i < 4)
            cHity->cd(i + 1);
        else
            cCalx31->cd(3);
        fh_Tpc_hity[i]->Draw("");
    }

    //  CANVAS 5  -------------------------------
    cHitxy = new TCanvas("TPC_position_XY", "TPC Hit info", 10, 10, 800, 700);
    cHitxy->Divide(2, 2);

    // Hit TPC data
    for (Int_t i = 0; i < 5; i++)
    { // one histo per detector
        sprintf(Name1, "fh_Tpc_hitxy_%d", i + 1);
        if (i < 2)
            sprintf(Name2, "XY for TPC 2%d at S2", i + 1);
        else if (i < 4)
            sprintf(Name2, "XY for TPC 4%d at S4", i - 1);
        else
            sprintf(Name2, "XY for TPC 31 at S4");
        fh_Tpc_hitxy[i] = new TH2F(Name1, Name2, 500, -100, 100., 500, -100, 100.);
        fh_Tpc_hitxy[i]->GetXaxis()->SetTitle("X [mm]");
        fh_Tpc_hitxy[i]->GetYaxis()->SetTitle("Y [mm]");
        fh_Tpc_hitxy[i]->GetXaxis()->CenterTitle(true);
        fh_Tpc_hitxy[i]->GetYaxis()->CenterTitle(true);
        fh_Tpc_hitxy[i]->GetYaxis()->SetTitleOffset(1.15);
        fh_Tpc_hitxy[i]->GetXaxis()->SetTitleOffset(1.1);
        fh_Tpc_hitxy[i]->GetXaxis()->SetLabelSize(0.045);
        fh_Tpc_hitxy[i]->GetXaxis()->SetTitleSize(0.045);
        fh_Tpc_hitxy[i]->GetYaxis()->SetLabelSize(0.045);
        fh_Tpc_hitxy[i]->GetYaxis()->SetTitleSize(0.045);
        if (i < 4)
            cHitxy->cd(i + 1);
        else
            cCalx31->cd(4);
        fh_Tpc_hitxy[i]->Draw("col");
    }

    //  CANVAS 6  -------------------------------
    c1ID = new TCanvas("FRS_ID", "frs info", 10, 10, 800, 700);

    // Analysis FRS data
    fh_Frs_Z = new TH1F("fh_Frs_Z", "Atomic number (Z)", 800, 40, 90.);
    fh_Frs_Z->GetXaxis()->SetTitle("Z");
    fh_Frs_Z->GetYaxis()->SetTitle("Counts");
    fh_Frs_Z->GetXaxis()->CenterTitle(true);
    fh_Frs_Z->GetYaxis()->CenterTitle(true);
    fh_Frs_Z->GetYaxis()->SetTitleOffset(1.1);
    fh_Frs_Z->GetXaxis()->SetTitleOffset(1.1);
    fh_Frs_Z->GetXaxis()->SetLabelSize(0.045);
    fh_Frs_Z->GetXaxis()->SetTitleSize(0.045);
    fh_Frs_Z->GetYaxis()->SetLabelSize(0.045);
    fh_Frs_Z->GetYaxis()->SetTitleSize(0.045);
    fh_Frs_Z->SetFillColor(4);
    fh_Frs_Z->SetLineColor(1);

    TPad* c1_1 = new TPad("pad11", fh_Frs_Z->GetName(), 0., 0.51, 0.49, 0.98);
    c1_1->Draw();
    c1_1->cd();
    fh_Frs_Z->Draw("");
    c1_1->Modified();
    c1ID->cd();

    fh_Frs_Aq = new TH1F("fh_Frs_Aq", "A/q for fragments", 500, 2.38, 2.63);
    fh_Frs_Aq->GetXaxis()->SetTitle("A/q");
    fh_Frs_Aq->GetYaxis()->SetTitle("Counts");
    fh_Frs_Aq->GetXaxis()->SetLabelSize(0.045);
    fh_Frs_Aq->GetXaxis()->SetTitleSize(0.045);
    fh_Frs_Aq->GetXaxis()->CenterTitle(true);
    fh_Frs_Aq->GetYaxis()->CenterTitle(true);
    fh_Frs_Aq->GetYaxis()->SetTitleOffset(1.1);
    fh_Frs_Aq->GetXaxis()->SetTitleOffset(1.1);
    fh_Frs_Aq->GetYaxis()->SetLabelSize(0.045);
    fh_Frs_Aq->GetYaxis()->SetTitleSize(0.045);
    fh_Frs_Aq->SetFillColor(4);
    fh_Frs_Aq->SetLineColor(1);

    TPad* c1_2 = new TPad("pad12", fh_Frs_Aq->GetName(), 0., 0.01, 0.49, 0.49);
    // c1_2->SetLogy();
    c1_2->Draw();
    c1_2->cd();
    fh_Frs_Aq->Draw("");
    c1_2->Modified();
    c1ID->cd();

    fh_Frs_ID = new TH2F("fh_Frs_ID", "FRS ID of fragments", 300, 2.45, 2.6, 200, 45, 55.);
    fh_Frs_ID->GetXaxis()->SetTitle("A/q");
    fh_Frs_ID->GetYaxis()->SetTitle("Atomic number (Z)");
    fh_Frs_ID->GetXaxis()->CenterTitle(true);
    fh_Frs_ID->GetYaxis()->CenterTitle(true);
    fh_Frs_ID->GetXaxis()->SetLabelSize(0.04);
    fh_Frs_ID->GetXaxis()->SetTitleSize(0.04);
    fh_Frs_ID->GetYaxis()->SetTitleOffset(1.1);
    fh_Frs_ID->GetXaxis()->SetTitleOffset(1.1);
    fh_Frs_ID->GetYaxis()->SetLabelSize(0.045);
    fh_Frs_ID->GetYaxis()->SetTitleSize(0.045);

    TPad* c1_3 = new TPad("pad13", fh_Frs_ID->GetName(), 0.51, 0.01, 0.98, 0.98);
    c1_3->Draw();
    c1_3->cd();
    fh_Frs_ID->Draw("col");
    c1_3->Modified();
    c1ID->cd();

    //  CANVAS 7  -------------------------------
    cSCI21 = new TCanvas("SCI21_E_raw", "sci21 info", 10, 10, 800, 700);

    // Analysis SCI21 data
    fh_sci21le = new TH1F("fh_sci21le", "SCI21-Left-Energy", 500, 0, 4092);
    fh_sci21le->GetXaxis()->SetTitle("Channels");
    fh_sci21le->GetYaxis()->SetTitle("Counts");
    fh_sci21le->GetXaxis()->CenterTitle(true);
    fh_sci21le->GetYaxis()->CenterTitle(true);
    fh_sci21le->GetYaxis()->SetTitleOffset(1.1);
    fh_sci21le->GetXaxis()->SetTitleOffset(1.1);
    fh_sci21le->GetXaxis()->SetLabelSize(0.045);
    fh_sci21le->GetXaxis()->SetTitleSize(0.045);
    fh_sci21le->GetYaxis()->SetLabelSize(0.045);
    fh_sci21le->GetYaxis()->SetTitleSize(0.045);
    fh_sci21le->SetFillColor(kGreen - 3);
    fh_sci21le->SetLineColor(1);

    TPad* c21 = new TPad("c21", fh_sci21le->GetName(), 0., 0.51, 0.49, 0.98);
    c21->Draw();
    c21->cd();
    fh_sci21le->Draw("");
    c21->Modified();
    cSCI21->cd();

    fh_sci21re = new TH1F("fh_sci21re", "SCI21-Right-Energy", 500, 0, 4092);
    fh_sci21re->GetXaxis()->SetTitle("Channels");
    fh_sci21re->GetYaxis()->SetTitle("Counts");
    fh_sci21re->GetXaxis()->CenterTitle(true);
    fh_sci21re->GetYaxis()->CenterTitle(true);
    fh_sci21re->GetYaxis()->SetTitleOffset(1.1);
    fh_sci21re->GetXaxis()->SetTitleOffset(1.1);
    fh_sci21re->GetXaxis()->SetLabelSize(0.045);
    fh_sci21re->GetXaxis()->SetTitleSize(0.045);
    fh_sci21re->GetYaxis()->SetLabelSize(0.045);
    fh_sci21re->GetYaxis()->SetTitleSize(0.045);
    fh_sci21re->SetFillColor(kGreen - 3);
    fh_sci21re->SetLineColor(1);

    TPad* c22 = new TPad("c22", fh_sci21re->GetName(), 0., 0.01, 0.49, 0.49);
    c22->Draw();
    c22->cd();
    fh_sci21re->Draw("");
    c22->Modified();
    cSCI21->cd();

    fh_sci21lere = new TH2F("fh_sci21lere", "SCI21-Energy", 200, 0, 4092, 200, 0, 4092);
    fh_sci21lere->GetXaxis()->SetTitle("Right [Channels]");
    fh_sci21lere->GetYaxis()->SetTitle("Left [Channels]");
    fh_sci21lere->GetXaxis()->CenterTitle(true);
    fh_sci21lere->GetYaxis()->CenterTitle(true);
    fh_sci21lere->GetXaxis()->SetLabelSize(0.03);
    fh_sci21lere->GetXaxis()->SetTitleSize(0.04);
    fh_sci21lere->GetYaxis()->SetTitleOffset(1.45);
    fh_sci21lere->GetXaxis()->SetTitleOffset(1.1);
    fh_sci21lere->GetYaxis()->SetLabelSize(0.03);
    fh_sci21lere->GetYaxis()->SetTitleSize(0.04);

    TPad* c23 = new TPad("c23", fh_sci21lere->GetName(), 0.51, 0.01, 0.98, 0.98);
    c23->Draw();
    c23->cd();
    c23->SetLeftMargin(0.14);
    fh_sci21lere->Draw("col");
    c23->Modified();
    cSCI21->cd();

    //  CANVAS 8  -------------------------------
    cSCI41 = new TCanvas("SCI41_E_raw", "sci41 info", 10, 10, 800, 700);

    // Analysis SCI41 data
    fh_sci41le = new TH1F("fh_sci41le", "SCI41-Left-Energy", 500, 0, 4092);
    fh_sci41le->GetXaxis()->SetTitle("Channels");
    fh_sci41le->GetYaxis()->SetTitle("Counts");
    fh_sci41le->GetXaxis()->CenterTitle(true);
    fh_sci41le->GetYaxis()->CenterTitle(true);
    fh_sci41le->GetYaxis()->SetTitleOffset(1.1);
    fh_sci41le->GetXaxis()->SetTitleOffset(1.1);
    fh_sci41le->GetXaxis()->SetLabelSize(0.045);
    fh_sci41le->GetXaxis()->SetTitleSize(0.045);
    fh_sci41le->GetYaxis()->SetLabelSize(0.045);
    fh_sci41le->GetYaxis()->SetTitleSize(0.045);
    fh_sci41le->SetFillColor(kGreen - 3);
    fh_sci41le->SetLineColor(1);

    TPad* c41 = new TPad("c41", fh_sci41le->GetName(), 0., 0.51, 0.49, 0.98);
    c41->Draw();
    c41->cd();
    fh_sci41le->Draw("");
    c41->Modified();
    cSCI41->cd();

    fh_sci41re = new TH1F("fh_sci41re", "SCI41-Right-Energy", 500, 0, 4092);
    fh_sci41re->GetXaxis()->SetTitle("Channels");
    fh_sci41re->GetYaxis()->SetTitle("Counts");
    fh_sci41re->GetXaxis()->CenterTitle(true);
    fh_sci41re->GetYaxis()->CenterTitle(true);
    fh_sci41re->GetYaxis()->SetTitleOffset(1.1);
    fh_sci41re->GetXaxis()->SetTitleOffset(1.1);
    fh_sci41re->GetXaxis()->SetLabelSize(0.045);
    fh_sci41re->GetXaxis()->SetTitleSize(0.045);
    fh_sci41re->GetYaxis()->SetLabelSize(0.045);
    fh_sci41re->GetYaxis()->SetTitleSize(0.045);
    fh_sci41re->SetFillColor(kGreen - 3);
    fh_sci41re->SetLineColor(1);

    TPad* c42 = new TPad("c42", fh_sci41re->GetName(), 0., 0.01, 0.49, 0.49);
    c42->Draw();
    c42->cd();
    fh_sci41re->Draw("");
    c42->Modified();
    cSCI41->cd();

    fh_sci41lere = new TH2F("fh_sci41lere", "SCI41-Energy", 200, 0, 4092, 200, 0, 4092);
    fh_sci41lere->GetXaxis()->SetTitle("Right [Channels]");
    fh_sci41lere->GetYaxis()->SetTitle("Left [Channels]");
    fh_sci41lere->GetXaxis()->CenterTitle(true);
    fh_sci41lere->GetYaxis()->CenterTitle(true);
    fh_sci41lere->GetXaxis()->SetLabelSize(0.03);
    fh_sci41lere->GetXaxis()->SetTitleSize(0.04);
    fh_sci41lere->GetYaxis()->SetTitleOffset(1.45);
    fh_sci41lere->GetXaxis()->SetTitleOffset(1.1);
    fh_sci41lere->GetYaxis()->SetLabelSize(0.03);
    fh_sci41lere->GetYaxis()->SetTitleSize(0.04);

    TPad* c43 = new TPad("c43", fh_sci41lere->GetName(), 0.51, 0.01, 0.98, 0.98);
    c43->Draw();
    c43->cd();
    c43->SetLeftMargin(0.14);
    fh_sci41lere->Draw("col");
    c43->Modified();
    cSCI41->cd();

    //  CANVAS 82  -------------------------------
    cSCI42 = new TCanvas("SCI42_E_raw", "sci42 info", 10, 10, 800, 700);

    // Analysis SCI41 data
    fh_sci42le = new TH1F("fh_sci42le", "SCI42-Left-Energy", 500, 0, 4092);
    fh_sci42le->GetXaxis()->SetTitle("Channels");
    fh_sci42le->GetYaxis()->SetTitle("Counts");
    fh_sci42le->GetXaxis()->CenterTitle(true);
    fh_sci42le->GetYaxis()->CenterTitle(true);
    fh_sci42le->GetYaxis()->SetTitleOffset(1.1);
    fh_sci42le->GetXaxis()->SetTitleOffset(1.1);
    fh_sci42le->GetXaxis()->SetLabelSize(0.045);
    fh_sci42le->GetXaxis()->SetTitleSize(0.045);
    fh_sci42le->GetYaxis()->SetLabelSize(0.045);
    fh_sci42le->GetYaxis()->SetTitleSize(0.045);
    fh_sci42le->SetFillColor(kGreen - 3);
    fh_sci42le->SetLineColor(1);

    TPad* c421 = new TPad("c421", fh_sci42le->GetName(), 0., 0.51, 0.49, 0.98);
    c421->Draw();
    c421->cd();
    fh_sci42le->Draw("");
    c421->Modified();
    cSCI42->cd();

    fh_sci42re = new TH1F("fh_sci42re", "SCI42-Right-Energy", 500, 0, 4092);
    fh_sci42re->GetXaxis()->SetTitle("Channels");
    fh_sci42re->GetYaxis()->SetTitle("Counts");
    fh_sci42re->GetXaxis()->CenterTitle(true);
    fh_sci42re->GetYaxis()->CenterTitle(true);
    fh_sci42re->GetYaxis()->SetTitleOffset(1.1);
    fh_sci42re->GetXaxis()->SetTitleOffset(1.1);
    fh_sci42re->GetXaxis()->SetLabelSize(0.045);
    fh_sci42re->GetXaxis()->SetTitleSize(0.045);
    fh_sci42re->GetYaxis()->SetLabelSize(0.045);
    fh_sci42re->GetYaxis()->SetTitleSize(0.045);
    fh_sci42re->SetFillColor(kGreen - 3);
    fh_sci42re->SetLineColor(1);

    TPad* c422 = new TPad("c422", fh_sci42re->GetName(), 0., 0.01, 0.49, 0.49);
    c422->Draw();
    c422->cd();
    fh_sci42re->Draw("");
    c422->Modified();
    cSCI42->cd();

    fh_sci42lere = new TH2F("fh_sci42lere", "SCI42-Energy", 200, 0, 4092, 200, 0, 4092);
    fh_sci42lere->GetXaxis()->SetTitle("Right [Channels]");
    fh_sci42lere->GetYaxis()->SetTitle("Left [Channels]");
    fh_sci42lere->GetXaxis()->CenterTitle(true);
    fh_sci42lere->GetYaxis()->CenterTitle(true);
    fh_sci42lere->GetXaxis()->SetLabelSize(0.03);
    fh_sci42lere->GetXaxis()->SetTitleSize(0.04);
    fh_sci42lere->GetYaxis()->SetTitleOffset(1.45);
    fh_sci42lere->GetXaxis()->SetTitleOffset(1.1);
    fh_sci42lere->GetYaxis()->SetLabelSize(0.03);
    fh_sci42lere->GetYaxis()->SetTitleSize(0.04);

    TPad* c423 = new TPad("c423", fh_sci42lere->GetName(), 0.51, 0.01, 0.98, 0.98);
    c423->Draw();
    c423->cd();
    c423->SetLeftMargin(0.14);
    fh_sci42lere->Draw("col");
    c423->Modified();
    cSCI42->cd();

    //  CANVAS 9  -------------------------------
    cSCI81 = new TCanvas("SCI81_E_raw", "sci81 info", 10, 10, 800, 700);

    // Analysis SCI81 data
    fh_sci81le = new TH1F("fh_sci81le", "SCI81-Left-Energy", 500, 0, 4092);
    fh_sci81le->GetXaxis()->SetTitle("Channels");
    fh_sci81le->GetYaxis()->SetTitle("Counts");
    fh_sci81le->GetXaxis()->CenterTitle(true);
    fh_sci81le->GetYaxis()->CenterTitle(true);
    fh_sci81le->GetYaxis()->SetTitleOffset(1.1);
    fh_sci81le->GetXaxis()->SetTitleOffset(1.1);
    fh_sci81le->GetXaxis()->SetLabelSize(0.045);
    fh_sci81le->GetXaxis()->SetTitleSize(0.045);
    fh_sci81le->GetYaxis()->SetLabelSize(0.045);
    fh_sci81le->GetYaxis()->SetTitleSize(0.045);
    fh_sci81le->SetFillColor(kGreen - 3);
    fh_sci81le->SetLineColor(1);

    TPad* c81 = new TPad("c81", fh_sci81le->GetName(), 0., 0.51, 0.49, 0.98);
    c81->Draw();
    c81->cd();
    fh_sci81le->Draw("");
    c81->Modified();
    cSCI81->cd();

    fh_sci81re = new TH1F("fh_sci81re", "SCI81-Right-Energy", 500, 0, 4092);
    fh_sci81re->GetXaxis()->SetTitle("Channels");
    fh_sci81re->GetYaxis()->SetTitle("Counts");
    fh_sci81re->GetXaxis()->CenterTitle(true);
    fh_sci81re->GetYaxis()->CenterTitle(true);
    fh_sci81re->GetYaxis()->SetTitleOffset(1.1);
    fh_sci81re->GetXaxis()->SetTitleOffset(1.1);
    fh_sci81re->GetXaxis()->SetLabelSize(0.045);
    fh_sci81re->GetXaxis()->SetTitleSize(0.045);
    fh_sci81re->GetYaxis()->SetLabelSize(0.045);
    fh_sci81re->GetYaxis()->SetTitleSize(0.045);
    fh_sci81re->SetFillColor(kGreen - 3);
    fh_sci81re->SetLineColor(1);

    TPad* c82 = new TPad("c82", fh_sci81re->GetName(), 0., 0.01, 0.49, 0.49);
    c82->Draw();
    c82->cd();
    fh_sci81re->Draw("");
    c82->Modified();
    cSCI81->cd();

    fh_sci81lere = new TH2F("fh_sci81lere", "SCI81-Energy", 200, 0, 4092, 200, 0, 4092);
    fh_sci81lere->GetXaxis()->SetTitle("Right [Channels]");
    fh_sci81lere->GetYaxis()->SetTitle("Left [Channels]");
    fh_sci81lere->GetXaxis()->CenterTitle(true);
    fh_sci81lere->GetYaxis()->CenterTitle(true);
    fh_sci81lere->GetXaxis()->SetLabelSize(0.03);
    fh_sci81lere->GetXaxis()->SetTitleSize(0.04);
    fh_sci81lere->GetYaxis()->SetTitleOffset(1.45);
    fh_sci81lere->GetXaxis()->SetTitleOffset(1.1);
    fh_sci81lere->GetYaxis()->SetLabelSize(0.03);
    fh_sci81lere->GetYaxis()->SetTitleSize(0.04);

    TPad* c83 = new TPad("c83", fh_sci81lere->GetName(), 0.51, 0.01, 0.98, 0.98);
    c83->Draw();
    c83->cd();
    c83->SetLeftMargin(0.14);
    fh_sci81lere->Draw("col");
    c83->Modified();
    cSCI81->cd();

    cSCI_21_41 = new TCanvas("SCI_21E_41E_raw", "SCI_21E_41E raw info", 10, 10, 800, 700);
    fh_sci2141E = new TH2F("fh_sci2141E", "Energy Sci 21 vs Energy Sci 41", 200, 0, 4092, 200, 0, 4092);
    fh_sci2141E->GetXaxis()->SetTitle("Energy Sci 21 [Channels]");
    fh_sci2141E->GetYaxis()->SetTitle("Energy Sci 41 [Channels]");
    fh_sci2141E->GetXaxis()->CenterTitle(true);
    fh_sci2141E->GetYaxis()->CenterTitle(true);
    fh_sci2141E->GetXaxis()->SetLabelSize(0.03);
    fh_sci2141E->GetXaxis()->SetTitleSize(0.04);
    fh_sci2141E->GetYaxis()->SetTitleOffset(1.2);
    fh_sci2141E->GetXaxis()->SetTitleOffset(1.1);
    fh_sci2141E->GetYaxis()->SetLabelSize(0.03);
    fh_sci2141E->GetYaxis()->SetTitleSize(0.04);
    fh_sci2141E->Draw("col");

    cSCI_21_81 = new TCanvas("SCI_21E_81E_raw", "SCI_21E_41E raw info", 10, 10, 800, 700);
    fh_sci2181E = new TH2F("fh_sci2181E", "Energy Sci 21 vs Energy Sci 81", 200, 0, 4092, 200, 0, 4092);
    fh_sci2181E->GetXaxis()->SetTitle("Energy Sci 21 [Channels]");
    fh_sci2181E->GetYaxis()->SetTitle("Energy Sci 81 [Channels]");
    fh_sci2181E->GetXaxis()->CenterTitle(true);
    fh_sci2181E->GetYaxis()->CenterTitle(true);
    fh_sci2181E->GetXaxis()->SetLabelSize(0.03);
    fh_sci2181E->GetXaxis()->SetTitleSize(0.04);
    fh_sci2181E->GetYaxis()->SetTitleOffset(1.2);
    fh_sci2181E->GetXaxis()->SetTitleOffset(1.1);
    fh_sci2181E->GetYaxis()->SetLabelSize(0.03);
    fh_sci2181E->GetYaxis()->SetTitleSize(0.04);
    fh_sci2181E->Draw("col");

    cSCI_tof2141R = new TCanvas("SCI_ToF_21R_41R_raw", "SCI_ToF_21R_41R raw info", 10, 10, 800, 700);
    fh_sci41rt = new TH1F("fh_sci41rt", "ToF_21_41 right", 200, 0, 4092);
    fh_sci41rt->GetXaxis()->SetTitle("ToF [Channels]");
    fh_sci41rt->GetYaxis()->SetTitle("Counts");
    fh_sci41rt->GetXaxis()->CenterTitle(true);
    fh_sci41rt->GetYaxis()->CenterTitle(true);
    fh_sci41rt->GetXaxis()->SetLabelSize(0.03);
    fh_sci41rt->GetXaxis()->SetTitleSize(0.04);
    fh_sci41rt->GetYaxis()->SetTitleOffset(1.2);
    fh_sci41rt->GetXaxis()->SetTitleOffset(1.1);
    fh_sci41rt->GetYaxis()->SetLabelSize(0.03);
    fh_sci41rt->GetYaxis()->SetTitleSize(0.04);
    fh_sci41rt->SetFillColor(kGreen - 3);
    fh_sci41rt->Draw("");

    cSCI_tof2141L = new TCanvas("SCI_ToF_21L_41L_raw", "SCI_ToF_21L_41L raw info", 10, 10, 800, 700);
    fh_sci41lt = new TH1F("fh_sci41lt", "ToF_21_41 left", 200, 0, 4092);
    fh_sci41lt->GetXaxis()->SetTitle("ToF [Channels]");
    fh_sci41lt->GetYaxis()->SetTitle("Counts");
    fh_sci41lt->GetXaxis()->CenterTitle(true);
    fh_sci41lt->GetYaxis()->CenterTitle(true);
    fh_sci41lt->GetXaxis()->SetLabelSize(0.03);
    fh_sci41lt->GetXaxis()->SetTitleSize(0.04);
    fh_sci41lt->GetYaxis()->SetTitleOffset(1.2);
    fh_sci41lt->GetXaxis()->SetTitleOffset(1.1);
    fh_sci41lt->GetYaxis()->SetLabelSize(0.03);
    fh_sci41lt->GetYaxis()->SetTitleSize(0.04);
    fh_sci41lt->SetFillColor(kGreen - 3);
    fh_sci41lt->Draw("");

    cSCI_tof2141 = new TCanvas("SCI_ToF_21_41_cal", "SCI_ToF_21_41 Cal info", 10, 10, 800, 700);
    fh_sci41t = new TH1F("fh_sci41t", "ToF_21_41_cal", 1000, 0, 62000);
    fh_sci41t->GetXaxis()->SetTitle("ToF [ps]");
    fh_sci41t->GetYaxis()->SetTitle("Counts");
    fh_sci41t->GetXaxis()->CenterTitle(true);
    fh_sci41t->GetYaxis()->CenterTitle(true);
    fh_sci41t->GetXaxis()->SetLabelSize(0.03);
    fh_sci41t->GetXaxis()->SetTitleSize(0.04);
    fh_sci41t->GetYaxis()->SetTitleOffset(1.2);
    fh_sci41t->GetXaxis()->SetTitleOffset(1.1);
    fh_sci41t->GetYaxis()->SetLabelSize(0.03);
    fh_sci41t->GetYaxis()->SetTitleSize(0.04);
    fh_sci41t->SetFillColor(kGreen - 3);
    fh_sci41t->Draw("");

    cSCI_tof21pos = new TCanvas("SCI_Pos_21_raw", "SCI_Pos_21 Cal info", 10, 10, 800, 700);
    fh_sci21pos = new TH1F("fh_sci21pos", "Position_21_raw", 4092, 0, 4092);
    fh_sci21pos->GetXaxis()->SetTitle("Position [ch]");
    fh_sci21pos->GetYaxis()->SetTitle("Counts");
    fh_sci21pos->GetXaxis()->CenterTitle(true);
    fh_sci21pos->GetYaxis()->CenterTitle(true);
    fh_sci21pos->GetXaxis()->SetLabelSize(0.03);
    fh_sci21pos->GetXaxis()->SetTitleSize(0.04);
    fh_sci21pos->GetYaxis()->SetTitleOffset(1.2);
    fh_sci21pos->GetXaxis()->SetTitleOffset(1.1);
    fh_sci21pos->GetYaxis()->SetLabelSize(0.03);
    fh_sci21pos->GetYaxis()->SetTitleSize(0.04);
    fh_sci21pos->SetFillColor(kGreen - 3);
    fh_sci21pos->Draw("");

    cSCI_tof41pos = new TCanvas("SCI_Pos_41_raw", "SCI_Pos_41 Cal info", 10, 10, 800, 700);
    fh_sci41pos = new TH1F("fh_sci41pos", "Position_41_raw", 4092, 0, 4092);
    fh_sci41pos->GetXaxis()->SetTitle("Position [ch]");
    fh_sci41pos->GetYaxis()->SetTitle("Counts");
    fh_sci41pos->GetXaxis()->CenterTitle(true);
    fh_sci41pos->GetYaxis()->CenterTitle(true);
    fh_sci41pos->GetXaxis()->SetLabelSize(0.03);
    fh_sci41pos->GetXaxis()->SetTitleSize(0.04);
    fh_sci41pos->GetYaxis()->SetTitleOffset(1.2);
    fh_sci41pos->GetXaxis()->SetTitleOffset(1.1);
    fh_sci41pos->GetYaxis()->SetLabelSize(0.03);
    fh_sci41pos->GetYaxis()->SetTitleSize(0.04);
    fh_sci41pos->SetFillColor(kGreen - 3);
    fh_sci41pos->Draw("");

    //  CANVAS 10  -------------------------------
    cSee = new TCanvas("SEETRAM", "seetram info", 10, 10, 800, 700);

    fh_Seetram = new TH1F("fh_Seetram", "Seetram rate per second", fseetram_range, 0, fseetram_range);
    fh_Seetram->GetXaxis()->SetTitle("Time [s]");
    fh_Seetram->GetYaxis()->SetTitle("Counts/s");
    fh_Seetram->GetXaxis()->CenterTitle(true);
    fh_Seetram->GetYaxis()->CenterTitle(true);
    fh_Seetram->GetYaxis()->SetTitleOffset(1.1);
    fh_Seetram->GetXaxis()->SetTitleOffset(1.1);
    fh_Seetram->GetXaxis()->SetLabelSize(0.045);
    fh_Seetram->GetXaxis()->SetTitleSize(0.045);
    fh_Seetram->GetYaxis()->SetLabelSize(0.045);
    fh_Seetram->GetYaxis()->SetTitleSize(0.045);
    fh_Seetram->SetFillColor(kGreen - 3);
    fh_Seetram->Draw();

    //  CANVAS 10  -------------------------------
    cSeet = new TCanvas("SEETRAM_test", "seetram test info", 10, 10, 800, 700);

    fh_Seetramt = new TH1F("fh_Seetramt", "Seetram_test rate per second", fseetram_range, 0, fseetram_range);
    fh_Seetramt->GetXaxis()->SetTitle("Time [s]");
    fh_Seetramt->GetYaxis()->SetTitle("Counts/s");
    fh_Seetramt->GetXaxis()->CenterTitle(true);
    fh_Seetramt->GetYaxis()->CenterTitle(true);
    fh_Seetramt->GetYaxis()->SetTitleOffset(1.1);
    fh_Seetramt->GetXaxis()->SetTitleOffset(1.1);
    fh_Seetramt->GetXaxis()->SetLabelSize(0.045);
    fh_Seetramt->GetXaxis()->SetTitleSize(0.045);
    fh_Seetramt->GetYaxis()->SetLabelSize(0.045);
    fh_Seetramt->GetYaxis()->SetTitleSize(0.045);
    fh_Seetramt->SetFillColor(kGreen - 3);
    fh_Seetramt->Draw();

    //  CANVAS 10  -------------------------------
    cIc = new TCanvas("IC", "IC info", 10, 10, 800, 700);

    fh_Ic = new TH1F("fh_Ic", "IC intensity per second", fseetram_range, 0, fseetram_range);
    fh_Ic->GetXaxis()->SetTitle("Time [s]");
    fh_Ic->GetYaxis()->SetTitle("Counts/s");
    fh_Ic->GetXaxis()->CenterTitle(true);
    fh_Ic->GetYaxis()->CenterTitle(true);
    fh_Ic->GetYaxis()->SetTitleOffset(1.1);
    fh_Ic->GetXaxis()->SetTitleOffset(1.1);
    fh_Ic->GetXaxis()->SetLabelSize(0.045);
    fh_Ic->GetXaxis()->SetTitleSize(0.045);
    fh_Ic->GetYaxis()->SetLabelSize(0.045);
    fh_Ic->GetYaxis()->SetTitleSize(0.045);
    fh_Ic->SetFillColor(kGreen - 3);
    fh_Ic->Draw();

    cSci00 = new TCanvas("SCI21_rate", "SCI21 Rate info", 10, 10, 800, 700);
    fh_Sci00 = new TH1F("fh_Sci21", "Sci21 rate per second", fseetram_range, 0, fseetram_range);
    fh_Sci00->GetXaxis()->SetTitle("Time [s]");
    fh_Sci00->GetYaxis()->SetTitle("Counts/s");
    fh_Sci00->GetXaxis()->CenterTitle(true);
    fh_Sci00->GetYaxis()->CenterTitle(true);
    fh_Sci00->GetYaxis()->SetTitleOffset(1.1);
    fh_Sci00->GetXaxis()->SetTitleOffset(1.1);
    fh_Sci00->GetXaxis()->SetLabelSize(0.045);
    fh_Sci00->GetXaxis()->SetTitleSize(0.045);
    fh_Sci00->GetYaxis()->SetLabelSize(0.045);
    fh_Sci00->GetYaxis()->SetTitleSize(0.045);
    fh_Sci00->SetFillColor(kGreen - 3);
    fh_Sci00->Draw();

    cSci01 = new TCanvas("SCI01", "SCI01 info", 10, 10, 800, 700);
    fh_Sci01 = new TH1F("fh_Sci01", "Sci01 rate per second", fseetram_range, 0, fseetram_range);
    fh_Sci01->GetXaxis()->SetTitle("Time [s]");
    fh_Sci01->GetYaxis()->SetTitle("Counts/s");
    fh_Sci01->GetXaxis()->CenterTitle(true);
    fh_Sci01->GetYaxis()->CenterTitle(true);
    fh_Sci01->GetYaxis()->SetTitleOffset(1.1);
    fh_Sci01->GetXaxis()->SetTitleOffset(1.1);
    fh_Sci01->GetXaxis()->SetLabelSize(0.045);
    fh_Sci01->GetXaxis()->SetTitleSize(0.045);
    fh_Sci01->GetYaxis()->SetLabelSize(0.045);
    fh_Sci01->GetYaxis()->SetTitleSize(0.045);
    fh_Sci01->SetFillColor(kGreen - 3);
    fh_Sci01->Draw();

    cSci02 = new TCanvas("SCI41_rate", "SCI41 Rate info", 10, 10, 800, 700);
    fh_Sci02 = new TH1F("fh_Sci41", "Sci41 rate per second", fseetram_range, 0, fseetram_range);
    fh_Sci02->GetXaxis()->SetTitle("Time [s]");
    fh_Sci02->GetYaxis()->SetTitle("Counts/s");
    fh_Sci02->GetXaxis()->CenterTitle(true);
    fh_Sci02->GetYaxis()->CenterTitle(true);
    fh_Sci02->GetYaxis()->SetTitleOffset(1.1);
    fh_Sci02->GetXaxis()->SetTitleOffset(1.1);
    fh_Sci02->GetXaxis()->SetLabelSize(0.045);
    fh_Sci02->GetXaxis()->SetTitleSize(0.045);
    fh_Sci02->GetYaxis()->SetLabelSize(0.045);
    fh_Sci02->GetYaxis()->SetTitleSize(0.045);
    fh_Sci02->SetFillColor(kGreen - 3);
    fh_Sci02->Draw();

    cSeeCom = new TCanvas("SEETRAM_comparison", "seetram comparison info", 10, 10, 800, 700);
    fh_SeetramC = new TH1F("fh_SeetramC", "Rates at S0", fseetram_rangeC, 0, fseetram_rangeC);
    fh_SeetramC->GetXaxis()->SetTitle("Time [s]");
    fh_SeetramC->GetYaxis()->SetTitle("Counts/s");
    fh_SeetramC->GetXaxis()->CenterTitle(true);
    fh_SeetramC->GetYaxis()->CenterTitle(true);
    fh_SeetramC->GetYaxis()->SetTitleOffset(1.1);
    fh_SeetramC->GetXaxis()->SetTitleOffset(1.1);
    fh_SeetramC->GetXaxis()->SetLabelSize(0.045);
    fh_SeetramC->GetXaxis()->SetTitleSize(0.045);
    fh_SeetramC->GetYaxis()->SetLabelSize(0.045);
    fh_SeetramC->GetYaxis()->SetTitleSize(0.045);
    fh_SeetramC->SetLineColor(4);
    fh_SeetramC->SetLineWidth(3);
    fh_SeetramC->SetStats(0);
    fh_SeetramC->Draw("");

    fh_SeetramtC = new TH1F("fh_SeetramtC", "", fseetram_rangeC, 0, fseetram_rangeC);
    fh_SeetramtC->SetLineColor(2);
    fh_SeetramtC->SetLineWidth(3);
    fh_SeetramtC->Draw("same");

    fh_IcC = new TH1F("fh_IcC", "", fseetram_rangeC, 0, fseetram_rangeC);
    fh_IcC->SetLineColor(1);
    fh_IcC->SetLineWidth(3);
    fh_IcC->Draw("same");

    fh_Sci00C = new TH1F("fh_SCI01C", "", fseetram_rangeC, 0, fseetram_rangeC);
    fh_Sci00C->SetLineColor(3);
    fh_Sci00C->SetLineWidth(3);
    fh_Sci00C->Draw("same");

    TLegend* leg = new TLegend(0.05, 0.9, 0.39, 0.9999, NULL, "brNDC");
    leg->SetBorderSize(0);
    leg->SetTextFont(62);
    leg->SetTextSize(0.03);
    leg->SetLineColor(1);
    leg->SetLineStyle(1);
    leg->SetLineWidth(1);
    leg->SetFillColor(0);
    leg->SetFillStyle(0);
    TLegendEntry* entry = leg->AddEntry("null", "Seetram", "l");
    entry->SetLineColor(4);
    entry->SetLineStyle(1);
    entry->SetLineWidth(3);
    entry->SetTextFont(62);
    entry = leg->AddEntry("null", "Seetram_test", "l");
    entry->SetLineColor(2);
    entry->SetLineStyle(1);
    entry->SetLineWidth(3);
    entry->SetTextFont(62);
    entry = leg->AddEntry("null", "IC", "l");
    entry->SetLineColor(1);
    entry->SetLineStyle(1);
    entry->SetLineWidth(3);
    entry->SetTextFont(62);
    entry = leg->AddEntry("null", "SCI01", "l");
    entry->SetLineColor(3);
    entry->SetLineStyle(1);
    entry->SetLineWidth(3);
    entry->SetTextFont(62);
    leg->Draw();

    cTrigCom = new TCanvas("Trigger_comparison", "Trigger comparison info", 10, 10, 800, 700);
    fh_TrigC = new TH1F("fh_Triggerc", "Rates", fseetram_rangeC, 0, fseetram_rangeC);
    fh_TrigC->GetXaxis()->SetTitle("Time [s]");
    fh_TrigC->GetYaxis()->SetTitle("Counts/s");
    fh_TrigC->GetXaxis()->CenterTitle(true);
    fh_TrigC->GetYaxis()->CenterTitle(true);
    fh_TrigC->GetYaxis()->SetTitleOffset(1.1);
    fh_TrigC->GetXaxis()->SetTitleOffset(1.1);
    fh_TrigC->GetXaxis()->SetLabelSize(0.045);
    fh_TrigC->GetXaxis()->SetTitleSize(0.045);
    fh_TrigC->GetYaxis()->SetLabelSize(0.045);
    fh_TrigC->GetYaxis()->SetTitleSize(0.045);
    fh_TrigC->SetLineColor(4);
    fh_TrigC->SetLineWidth(3);
    fh_TrigC->SetStats(0);
    fh_TrigC->Draw("");

    fh_TrigFree = new TH1F("fh_TrigFree", "", fseetram_rangeC, 0, fseetram_rangeC);
    fh_TrigFree->SetLineColor(2);
    fh_TrigFree->SetLineWidth(3);
    fh_TrigFree->Draw("same");

    leg = new TLegend(0.05, 0.9, 0.39, 0.9999, NULL, "brNDC");
    leg->SetBorderSize(0);
    leg->SetTextFont(62);
    leg->SetTextSize(0.035);
    leg->SetLineColor(1);
    leg->SetLineStyle(1);
    leg->SetLineWidth(1);
    leg->SetFillColor(0);
    leg->SetFillStyle(0);
    entry = leg->AddEntry("null", "Accepted triggers", "l");
    entry->SetLineColor(4);
    entry->SetLineStyle(1);
    entry->SetLineWidth(3);
    entry->SetTextFont(62);
    entry = leg->AddEntry("null", "Free triggers", "l");
    entry->SetLineColor(2);
    entry->SetLineStyle(1);
    entry->SetLineWidth(3);
    entry->SetTextFont(62);
    leg->Draw();

    //  CANVAS 11  -------------------------------
    cTransS2 = new TCanvas("Position_Angle_S2", "Transmission info at S2", 10, 10, 800, 700);

    fh_ts2 = new TH2F("fh_ts2", "Focal Plane at S2", 500, -100, 100, 500, -18, 18);
    fh_ts2->GetXaxis()->SetTitle("Position [mm]");
    fh_ts2->GetYaxis()->SetTitle("Angle [mrad]");
    fh_ts2->GetXaxis()->CenterTitle(true);
    fh_ts2->GetYaxis()->CenterTitle(true);
    fh_ts2->GetXaxis()->SetLabelSize(0.03);
    fh_ts2->GetXaxis()->SetTitleSize(0.04);
    fh_ts2->GetYaxis()->SetTitleOffset(1.15);
    fh_ts2->GetXaxis()->SetTitleOffset(1.1);
    fh_ts2->GetYaxis()->SetLabelSize(0.03);
    fh_ts2->GetYaxis()->SetTitleSize(0.04);
    fh_ts2->Draw("col");

    cTrackS2 = new TCanvas("Tracking_S2", "Tracking info at S2", 10, 10, 800, 700);
    fh_tr2 = new TH2F("fh_tr2", "Tracking at S2", 200, 0, 4100, 200, -100, 100);
    fh_tr2->GetXaxis()->SetTitle("Position Z [mm]");
    fh_tr2->GetYaxis()->SetTitle("Position X [mm]");
    fh_tr2->GetXaxis()->CenterTitle(true);
    fh_tr2->GetYaxis()->CenterTitle(true);
    fh_tr2->GetXaxis()->SetLabelSize(0.03);
    fh_tr2->GetXaxis()->SetTitleSize(0.04);
    fh_tr2->GetYaxis()->SetTitleOffset(1.15);
    fh_tr2->GetXaxis()->SetTitleOffset(1.1);
    fh_tr2->GetYaxis()->SetLabelSize(0.03);
    fh_tr2->GetYaxis()->SetTitleSize(0.04);
    fh_tr2->Draw("col");

    //  CANVAS 12  -------------------------------
    cTransS4 = new TCanvas("Position_Angle_S4", "Transmission info at S4", 10, 10, 800, 700);
    fh_ts4 = new TH2F("fh_ts4", "Focal Plane at S4", 500, -100, 100, 500, -18, 18);
    fh_ts4->GetXaxis()->SetTitle("Position [mm]");
    fh_ts4->GetYaxis()->SetTitle("Angle [mrad]");
    fh_ts4->GetXaxis()->CenterTitle(true);
    fh_ts4->GetYaxis()->CenterTitle(true);
    fh_ts4->GetXaxis()->SetLabelSize(0.03);
    fh_ts4->GetXaxis()->SetTitleSize(0.04);
    fh_ts4->GetYaxis()->SetTitleOffset(1.15);
    fh_ts4->GetXaxis()->SetTitleOffset(1.1);
    fh_ts4->GetYaxis()->SetLabelSize(0.03);
    fh_ts4->GetYaxis()->SetTitleSize(0.04);
    fh_ts4->Draw("col");

    cTrackS4 = new TCanvas("Tracking_S4", "Tracking info at S4", 10, 10, 800, 700);
    fh_tr4 = new TH2F("fh_tr4", "Tracking at S4", 200, 0, 3000, 200, -100, 100);
    fh_tr4->GetXaxis()->SetTitle("Position Z [mm]");
    fh_tr4->GetYaxis()->SetTitle("Position X [mm]");
    fh_tr4->GetXaxis()->CenterTitle(true);
    fh_tr4->GetYaxis()->CenterTitle(true);
    fh_tr4->GetXaxis()->SetLabelSize(0.03);
    fh_tr4->GetXaxis()->SetTitleSize(0.04);
    fh_tr4->GetYaxis()->SetTitleOffset(1.15);
    fh_tr4->GetXaxis()->SetTitleOffset(1.1);
    fh_tr4->GetYaxis()->SetLabelSize(0.03);
    fh_tr4->GetYaxis()->SetTitleSize(0.04);
    fh_tr4->Draw("col");

    // MWs
    cMW1 = new TCanvas("Position_MW_S1_to_S3", "MW positions", 10, 10, 800, 700);
    cMW1->Divide(2, 2);
    cMW2 = new TCanvas("Position_MW_S3_to_S8", "MW positions", 10, 10, 800, 700);
    cMW2->Divide(2, 2);
    TString name[13] = { "MW11", "MW21", "MW22", "MW31", "MW41", "MW42", "MW51",
                         "MW61", "MW71", "MW81", "MW82", "MWB1", "MWB2" };
    for (Int_t i = 0; i < 13; i++)
    {
        fh_mw[i] = new TH2F("fh_" + name[i], name[i], 50, -100, 100, 50, -100, 100);
        fh_mw[i]->GetXaxis()->SetTitle("x [mm]");
        fh_mw[i]->GetYaxis()->SetTitle("y [mm]");
        fh_mw[i]->GetXaxis()->CenterTitle(true);
        fh_mw[i]->GetYaxis()->CenterTitle(true);
        fh_mw[i]->GetXaxis()->SetLabelSize(0.04);
        fh_mw[i]->GetXaxis()->SetTitleSize(0.04);
        fh_mw[i]->GetXaxis()->SetTitleOffset(1.1);
        fh_mw[i]->GetYaxis()->SetLabelSize(0.04);
        fh_mw[i]->GetYaxis()->SetTitleSize(0.04);
        fh_mw[i]->GetYaxis()->SetTitleOffset(1.1);
        if (i < 4)
        {
            cMW1->cd(i + 1);
            fh_mw[i]->Draw("col");
        }
        if (name[i] == "MW51")
        {
            cMW2->cd(1);
            fh_mw[i]->Draw("col");
        }
        if (name[i] == "MW71")
        {
            cMW2->cd(2);
            fh_mw[i]->Draw("col");
        }
        if (name[i] == "MW81")
        {
            cMW2->cd(3);
            fh_mw[i]->Draw("col");
        }
        if (name[i] == "MW82")
        {
            cMW2->cd(4);
            fh_mw[i]->Draw("col");
        }
    }

    //
    cTrigger = new TCanvas("Triggers", "Trigger information", 10, 10, 800, 700);
    fh_trigger = new TH1F("fh_trigger", "Trigger information", 16, -0.5, 15.5);
    fh_trigger->GetXaxis()->SetTitle("Trigger number");
    fh_trigger->GetYaxis()->SetTitle("Counts");
    fh_trigger->GetXaxis()->CenterTitle(true);
    fh_trigger->GetYaxis()->CenterTitle(true);
    fh_trigger->GetXaxis()->SetLabelSize(0.04);
    fh_trigger->GetXaxis()->SetTitleSize(0.04);
    fh_trigger->GetYaxis()->SetTitleOffset(1.1);
    fh_trigger->GetXaxis()->SetTitleOffset(1.1);
    fh_trigger->GetYaxis()->SetLabelSize(0.04);
    fh_trigger->GetYaxis()->SetTitleSize(0.04);
    fh_trigger->SetFillColor(kBlue + 2);
    fh_trigger->Draw("");

    // Difference between WRs
    sprintf(Name1, "WR_TimeStamp");
    sprintf(Name2, "fh_WR_Master");
    cWr = new TCanvas(Name1, "WR Time stamp", 10, 10, 500, 500);
    fh_wr = new TH1F(Name2, "WR Time stamp for General DAQ", 100000, 0, fWrrange);
    fh_wr->GetXaxis()->SetTitle("(WR-First_WR) [s]");
    fh_wr->GetXaxis()->SetTitleOffset(1.15);
    fh_wr->GetYaxis()->SetTitle("Counts");
    fh_wr->GetYaxis()->SetTitleOffset(1.15);
    fh_wr->GetXaxis()->CenterTitle(true);
    fh_wr->GetYaxis()->CenterTitle(true);
    fh_wr->SetFillColor(kBlue + 2);
    fh_wr->Draw("");

    // FOLDERS
    TFolder* mainfolMu = new TFolder("MUSIC", "Music info");
    mainfolMu->Add(cMus1);
    mainfolMu->Add(cMus2);
    mainfolMu->Add(cMus3);
    mainfolMu->Add(cMu41_mu42);
    mainfolMu->Add(cMus1cal);
    mainfolMu->Add(cMus2cal);
    mainfolMu->Add(cMus3cal);
    for (Int_t i = 0; i < 3; i++)
        mainfolMu->Add(cMushit[i]);

    TFolder* mainfolMw = new TFolder("MW", "MW info");
    mainfolMw->Add(cMW1);
    mainfolMw->Add(cMW2);

    TFolder* mainfolTpc = new TFolder("TPC", "TPC info");
    mainfolTpc->Add(cHitx);
    mainfolTpc->Add(cHity);
    mainfolTpc->Add(cHitxy);
    mainfolTpc->Add(cTpcCal[0]);
    mainfolTpc->Add(cTpcCal[1]);
    mainfolTpc->Add(cTpcCal[4]);
    mainfolTpc->Add(cTpcCal[2]);
    mainfolTpc->Add(cTpcCal[3]);
    mainfolTpc->Add(cCalx);
    mainfolTpc->Add(cCalx31);
    mainfolTpc->Add(cTransS2);
    mainfolTpc->Add(cTransS4);
    mainfolTpc->Add(cTrackS2);
    mainfolTpc->Add(cTrackS4);

    TFolder* mainfolSci = new TFolder("SCI", "SCI info");
    mainfolSci->Add(cSci00);
    mainfolSci->Add(cSci02);
    mainfolSci->Add(cSCI21);
    mainfolSci->Add(cSCI41);
    mainfolSci->Add(cSCI42);
    mainfolSci->Add(cSCI81);
    mainfolSci->Add(cSCI_21_41);
    mainfolSci->Add(cSCI_21_81);
    mainfolSci->Add(cSCI_tof2141R);
    mainfolSci->Add(cSCI_tof2141L);
    mainfolSci->Add(cSCI_tof2141);
    mainfolSci->Add(cSCI_tof21pos);
    mainfolSci->Add(cSCI_tof41pos);

    TFolder* mainfolSee = new TFolder("Detectors@S0", "SEETRAM, IC and SCI info at S0");
    mainfolSee->Add(cSee);
    mainfolSee->Add(cSeet);
    mainfolSee->Add(cIc);
    // mainfolSee->Add(cSci00);
    mainfolSee->Add(cSci01);
    // mainfolSee->Add(cSci02);
    mainfolSee->Add(cSeeCom);

    // MAIN FOLDER-FRS
    TFolder* mainfolFrs = new TFolder("FRS", "FRS info");
    mainfolFrs->Add(cTrigger);
    mainfolFrs->Add(cWr);
    mainfolFrs->Add(cTrigCom);
    mainfolFrs->Add(mainfolSee);
    mainfolFrs->Add(mainfolSci);
    mainfolFrs->Add(mainfolTpc);
    mainfolFrs->Add(mainfolMu);
    mainfolFrs->Add(mainfolMw);
    mainfolFrs->Add(c1ID);
    run->AddObject(mainfolFrs);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_ALL_HIST", Form("/Objects/%s/->Reset_GENERAL_Histo()", GetName()));
    run->GetHttpServer()->RegisterCommand("Reset_Detectors@SO", Form("/Objects/%s/->Reset_SEETRAM_Histo()", GetName()));
    run->GetHttpServer()->RegisterCommand("Reset_MUSICs", Form("/Objects/%s/->Reset_MUSIC_Histo()", GetName()));
    run->GetHttpServer()->RegisterCommand("Reset_TPCs", Form("/Objects/%s/->Reset_TPC_Histo()", GetName()));
    run->GetHttpServer()->RegisterCommand("Reset_SCIs", Form("/Objects/%s/->Reset_SCI_Histo()", GetName()));
    run->GetHttpServer()->RegisterCommand("Reset_MWs", Form("/Objects/%s/->Reset_MW_Histo()", GetName()));
    run->GetHttpServer()->RegisterCommand("Reset_FRS_ID", Form("/Objects/%s/->Reset_FRS_Histo()", GetName()));

    return kSUCCESS;
}

void R3BFrsOnlineSpectra::Reset_SEETRAM_Histo()
{
    LOG(INFO) << "R3BFrsOnlineSpectra::Reset_Detectors@So_Histo";
    fh_Seetram->Reset();
    fh_Seetramt->Reset();
    fh_Ic->Reset();
    // fh_Sci00->Reset();
    fh_Sci01->Reset();
    // fh_Sci02->Reset();

    fh_SeetramC->Reset();
    fh_SeetramtC->Reset();
    fh_IcC->Reset();
    fh_Sci00C->Reset();
}

void R3BFrsOnlineSpectra::Reset_FRS_Histo()
{
    LOG(INFO) << "R3BFrsOnlineSpectra::Reset_FRS_Histo";

    fh_Frs_Z->Reset();
    fh_Frs_Aq->Reset();
    fh_Frs_ID->Reset();
}

void R3BFrsOnlineSpectra::Reset_MW_Histo()
{
    LOG(INFO) << "R3BFrsOnlineSpectra::Reset_MW_Histo";
    for (Int_t i = 0; i < 13; i++)
    {
        fh_mw[i]->Reset();
    }
}

void R3BFrsOnlineSpectra::Reset_SCI_Histo()
{
    LOG(INFO) << "R3BFrsOnlineSpectra::Reset_SCI_Histo";
    fh_Sci00->Reset();
    fh_Sci02->Reset();
    fh_sci21le->Reset();
    fh_sci21re->Reset();
    fh_sci21lere->Reset();
    fh_sci41le->Reset();
    fh_sci41re->Reset();
    fh_sci41lere->Reset();
    fh_sci42le->Reset();
    fh_sci42re->Reset();
    fh_sci42lere->Reset();
    fh_sci81le->Reset();
    fh_sci81re->Reset();
    fh_sci81lere->Reset();
    fh_sci2141E->Reset();
    fh_sci2181E->Reset();
    fh_sci41lt->Reset();
    fh_sci41rt->Reset();
    fh_sci41t->Reset();
    fh_sci21pos->Reset();
    fh_sci41pos->Reset();
}

void R3BFrsOnlineSpectra::Reset_MUSIC_Histo()
{
    LOG(INFO) << "R3BFrsOnlineSpectra::Reset_MUSIC_Histo";
    // Cal data
    for (Int_t i = 0; i < 24; i++)
    {
        fh_music_energy_per_anode[i]->Reset();
        fh_music_energy_per_anodecal[i]->Reset();
    }
    for (Int_t i = 0; i < 3; i++)
        fh_music_Z[i]->Reset();
    fh_mu41_mu42->Reset();
}

void R3BFrsOnlineSpectra::Reset_TPC_Histo()
{
    LOG(INFO) << "R3BFrsOnlineSpectra::Reset_TPC_Histo";
    // Hit data
    for (Int_t i = 0; i < 5; i++)
    {
        fh_Tpc_hitx[i]->Reset();
        fh_Tpc_hity[i]->Reset();
        fh_Tpc_hitxy[i]->Reset();
        fh_tpc_deltax[i]->Reset();
    }
    fh_tr2->Reset();
    fh_ts2->Reset();
    fh_ts4->Reset();
    fh_tr4->Reset();
    // Cal data
    for (Int_t i = 0; i < 20; i++)
        fh_tpc_csum[i]->Reset();
}

void R3BFrsOnlineSpectra::Reset_GENERAL_Histo()
{
    LOG(INFO) << "R3BFrsOnlineSpectra::Reset_General_Histo";
    fh_trigger->Reset();
    fh_TrigC->Reset();
    fh_TrigFree->Reset();
    fh_wr->Reset();
    fFirstWr = kTRUE;
    Reset_SEETRAM_Histo();
    Reset_TPC_Histo();
    Reset_MUSIC_Histo();
    Reset_SCI_Histo();
    Reset_MW_Histo();
    Reset_FRS_Histo();
}

void R3BFrsOnlineSpectra::Exec(Option_t* option)
{
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(FATAL) << "R3BFrsOnlineSpectra::Exec FairRootManager not found";

    // Fill wr-general-DAQ
    if (fWRItemsMaster && fWRItemsMaster->GetEntriesFast())
    {
        uint64_t wrm = 0;
        Int_t nHits = fWRItemsMaster->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BWRMasterData* hit = (R3BWRMasterData*)fWRItemsMaster->At(ihit);
            if (!hit)
                continue;
            if (fFirstWr)
            {
                fFirstWr = kFALSE;
                fFirstValueWR = hit->GetTimeStamp();
            }
            // if(uint64_t((wrm-fFirstValueWR)/1.e12)>uint64_t(fWrrange))fFirstValueWR=hit->GetTimeStamp();
            wrm = hit->GetTimeStamp();
            // std::cout <<(wrm-fFirstValueWR)/1.e12 <<std::endl;
        }
        fh_wr->Fill((wrm - fFirstValueWR) / 1.e12);
    }

    // Fill mapped data
    if (fMappedItemsFrs && fMappedItemsFrs->GetEntriesFast())
    {
        Int_t nHits = fMappedItemsFrs->GetEntriesFast();
        Double_t tofrr = 0, tofll = 0;
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BFrsMappedData* hit = (R3BFrsMappedData*)fMappedItemsFrs->At(ihit);
            if (!hit)
                continue;
            fh_sci21le->Fill(hit->GetSCI21LE());
            fh_sci21re->Fill(hit->GetSCI21RE());
            fh_sci21lere->Fill(hit->GetSCI21RE(), hit->GetSCI21LE());
            fh_sci41le->Fill(hit->GetSCI41LE());
            fh_sci41re->Fill(hit->GetSCI41RE());
            fh_sci41lere->Fill(hit->GetSCI41RE(), hit->GetSCI41LE());

            fh_sci42le->Fill(hit->GetSCI42LE());
            fh_sci42re->Fill(hit->GetSCI42RE());
            fh_sci42lere->Fill(hit->GetSCI42RE(), hit->GetSCI42LE());

            fh_sci81le->Fill(hit->GetSCI81LE());
            fh_sci81re->Fill(hit->GetSCI81RE());
            fh_sci81lere->Fill(hit->GetSCI81RE(), hit->GetSCI81LE());

            fh_sci41rt->Fill(hit->GetSCI41RT());
            fh_sci41lt->Fill(hit->GetSCI41LT());

            tofll = hit->GetSCI41LT() * 11.0988 + 538.608;
            tofrr = hit->GetSCI41RT() * 10.9513 - 559.745;

            if (tofrr > 2500 && tofll > 2500)
                fh_sci41t->Fill(0.5 * (tofrr + tofll));

            if (hit->GetSCI21LT() > 200)
                fh_sci21pos->Fill(hit->GetSCI21LT());
            if (hit->GetSCI21RT() > 200)
                fh_sci41pos->Fill(hit->GetSCI21RT());

            fh_trigger->Fill(hit->GetTrigger());
            fh_sci2141E->Fill(sqrt(hit->GetSCI21RE() * hit->GetSCI21LE()), sqrt(hit->GetSCI41RE() * hit->GetSCI41LE()));
            fh_sci2181E->Fill(sqrt(hit->GetSCI21RE() * hit->GetSCI21LE()), sqrt(hit->GetSCI81RE() * hit->GetSCI81LE()));
        }
    }

    // Fill cal seetram data
    if (fCalItemsSeetram && fCalItemsSeetram->GetEntriesFast())
    {
        Int_t nHits = fCalItemsSeetram->GetEntriesFast();
        // std::cout << "hit:"<<nHits << std::endl;
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BSeetramCalData* hit = (R3BSeetramCalData*)fCalItemsSeetram->At(ihit);
            if (!hit)
                continue;
            // std::cout << "hit:"<<hit->GetIcCounts() << " " << hit->GetSeeCounts()<< " " << hit->GetAccTrigCounts() <<
            // std::endl;
            if (hit->GetClock1seg() + fOffsetSeetram > fseetram_range)
                for (int j = 0; j < 10000; j++)
                    if (hit->GetClock1seg() > fseetram_range * j && hit->GetClock1seg() < fseetram_range * (j + 1))
                    {
                        fOffsetSeetram = -fseetram_range * j;
                        fh_Seetram->Reset();
                        fh_Seetramt->Reset();
                        fh_Ic->Reset();
                        fh_Sci00->Reset();
                        fh_Sci01->Reset();
                        fh_Sci02->Reset();
                        break;
                    }
            if (hit->GetClock1seg() + fOffsetSeetramC > fseetram_rangeC)
                for (int j = 0; j < 10000; j++)
                    if (hit->GetClock1seg() > fseetram_rangeC * j && hit->GetClock1seg() < fseetram_rangeC * (j + 1))
                    {
                        fOffsetSeetramC = -fseetram_rangeC * j;
                        fh_TrigC->Reset();
                        fh_TrigFree->Reset();
                        fh_SeetramC->Reset();
                        fh_SeetramtC->Reset();
                        fh_IcC->Reset();
                        fh_Sci00C->Reset();
                        break;
                    }
            fh_Seetram->SetBinContent(hit->GetClock1seg() + fOffsetSeetram, hit->GetSeeCounts());
            fh_Seetramt->SetBinContent(hit->GetClock1seg() + fOffsetSeetram, hit->GetDumCounts());
            fh_Ic->SetBinContent(hit->GetClock1seg() + fOffsetSeetram, hit->GetIcCounts());
            fh_Sci00->SetBinContent(hit->GetClock1seg() + fOffsetSeetram, hit->GetSci00Counts());
            fh_Sci01->SetBinContent(hit->GetClock1seg() + fOffsetSeetram, hit->GetSci01Counts());
            fh_Sci02->SetBinContent(hit->GetClock1seg() + fOffsetSeetram, hit->GetSci02Counts());

            Int_t maxtrig = TMath::Max(hit->GetAccTrigCounts(), hit->GetFreeTrigCounts());
            fh_TrigC->SetMaximum(maxtrig * 1.2);
            fh_TrigC->SetBinContent(hit->GetClock1seg() + fOffsetSeetramC, hit->GetAccTrigCounts()); // FIXME: ranges!!
            fh_TrigFree->SetBinContent(hit->GetClock1seg() + fOffsetSeetramC,
                                       hit->GetFreeTrigCounts()); // FIXME: ranges!!

            Int_t max = TMath::Max(TMath::Max(hit->GetIcCounts(), hit->GetSci01Counts()),
                                   TMath::Max(hit->GetSeeCounts(), hit->GetDumCounts()));
            fh_SeetramC->SetBinContent(hit->GetClock1seg() + fOffsetSeetramC, hit->GetSeeCounts());
            fh_SeetramtC->SetBinContent(hit->GetClock1seg() + fOffsetSeetramC, hit->GetDumCounts());
            fh_IcC->SetBinContent(hit->GetClock1seg() + fOffsetSeetramC, hit->GetIcCounts());
            fh_Sci00C->SetBinContent(hit->GetClock1seg() + fOffsetSeetramC, hit->GetSci01Counts());
            fh_SeetramC->SetMaximum(max * 1.2);
            fh_SeetramtC->SetMaximum(max * 1.2);
            fh_IcC->SetMaximum(max * 1.2);
            fh_Sci00C->SetMaximum(max * 1.2);
        }
    }

    // Fill map music data
    if (fMapItemsMusic && fMapItemsMusic->GetEntriesFast())
    {
        Int_t nHits = fMapItemsMusic->GetEntriesFast();
        // std::cout << "hit:"<<nHits << std::endl;
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BMusicMappedData* hit = (R3BMusicMappedData*)fMapItemsMusic->At(ihit);
            if (!hit)
                continue;
            // std::cout << "hit:"<<hit->GetDetectorId() << " " << hit->GetEnergy() << std::endl;
            fh_music_energy_per_anode[hit->GetDetectorId() * 8 + hit->GetAnodeId()]->Fill(hit->GetEnergy());
        }
    }

    // Fill cal music data
    if (fCalItemsMusic && fCalItemsMusic->GetEntriesFast())
    {
        Int_t nHits = fCalItemsMusic->GetEntriesFast();
        // std::cout << "hit:"<<nHits << std::endl;
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BMusicCalData* hit = (R3BMusicCalData*)fCalItemsMusic->At(ihit);
            if (!hit)
                continue;
            // std::cout << "hit:"<<hit->GetDetectorId() << " " << hit->GetEnergy() << std::endl;
            fh_music_energy_per_anodecal[hit->GetDetectorId() * 8 + hit->GetAnodeId()]->Fill(hit->GetEnergy());
        }
    }

    // Fill hit music data
    if (fHitItemsMusic && fHitItemsMusic->GetEntriesFast())
    {
        Int_t nHits = fHitItemsMusic->GetEntriesFast();
        // std::cout << "hit:"<<nHits << std::endl;
        double z41 = 0, z42 = 0;
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BMusicHitData* hit = (R3BMusicHitData*)fHitItemsMusic->At(ihit);
            if (!hit)
                continue;
            fh_music_Z[hit->GetDetectorId()]->Fill(hit->GetZ());
            if (hit->GetDetectorId() == 0)
                z41 = hit->GetZ();
            if (hit->GetDetectorId() == 2)
                z42 = hit->GetZ();
        }
        if (z41 > 1 && z42 > 1)
            fh_mu41_mu42->Fill(z41 * z41, z42 * z42);
    }

    // Fill cal tpc data
    Int_t deltaX[10];
    if (fCalItemsTpc && fCalItemsTpc->GetEntriesFast())
    {
        Int_t nHits = fCalItemsTpc->GetEntriesFast();
        // std::cout << nHits << std::endl;
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BTpcCalData* hit = (R3BTpcCalData*)fCalItemsTpc->At(ihit);
            if (!hit)
                continue;
            if (hit->GetXYId())
            {
                fh_tpc_csum[hit->GetDetectorId() * 4 + hit->GetSecId()]->Fill(hit->GetControlPar());
            }
            else
            {
                deltaX[hit->GetDetectorId() * 2 + hit->GetSecId()] = hit->GetControlPar();
            }
        }
        // DeltaX for TPCs: X1-X0
        fh_tpc_deltax[0]->Fill(deltaX[1] - deltaX[0]);
        fh_tpc_deltax[1]->Fill(deltaX[3] - deltaX[2]);
        fh_tpc_deltax[2]->Fill(deltaX[5] - deltaX[4]);
        fh_tpc_deltax[3]->Fill(deltaX[7] - deltaX[6]);
    }

    // Fill hit tpc data
    if (fHitItemsTpc && fHitItemsTpc->GetEntriesFast())
    {
        Int_t nHits = fHitItemsTpc->GetEntriesFast();
        // std::cout << nHits << std::endl;
        TVector3 master[5];
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BTpcHitData* hit = (R3BTpcHitData*)fHitItemsTpc->At(ihit);
            if (!hit)
                continue;
            fh_Tpc_hitx[hit->GetDetectorId()]->Fill(hit->GetX());
            fh_Tpc_hity[hit->GetDetectorId()]->Fill(hit->GetY());
            fh_Tpc_hitxy[hit->GetDetectorId()]->Fill(hit->GetX(), hit->GetY());
            master[hit->GetDetectorId()].SetXYZ(tpcpos[hit->GetDetectorId()], hit->GetX(), 0.);
        }
        double zr = 0.;
        fh_ts2->Fill(master[0].Y() + (master[1] - master[0]).Phi() * 2000., (master[1] - master[0]).Phi() * 1000.);
        fh_ts4->Fill(master[2].Y() + (master[3] - master[2]).Phi() * 1500., (master[3] - master[2]).Phi() * 1000.);
        for (Int_t j = 0; j < 10; j++)
        {
            zr = gRandom->Uniform(0., 4050.);
            if (master[0].Y() > -100. && master[0].Y() < 100. && master[1].Y() > -100. && master[1].Y() < 100. &&
                abs((master[1] - master[0]).Phi()) < 0.016)
                fh_tr2->Fill(zr, master[0].Y() + (master[1] - master[0]).Phi() * zr);
            if (master[2].Y() > -100. && master[2].Y() < 100. && master[3].Y() > -100. && master[3].Y() < 100. &&
                abs((master[3] - master[2]).Phi()) < 0.016)
                fh_tr4->Fill(zr, master[2].Y() + (master[3] - master[2]).Phi() * zr);
        }
    }

    // Fill hit mw data
    if (fHitItemsMw && fHitItemsMw->GetEntriesFast())
    {
        Int_t nHits = fHitItemsMw->GetEntriesFast();
        // std::cout << nHits << std::endl;
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BMwHitData* hit = (R3BMwHitData*)fHitItemsMw->At(ihit);
            if (!hit)
                continue;
            // std::cout << hit->GetDetId() <<" " <<hit->GetX() << std::endl;
            fh_mw[hit->GetDetId()]->Fill(hit->GetX(), hit->GetY());
        }
    }

    // Fill FRS data
    if (fAnaItemsFrs && fAnaItemsFrs->GetEntriesFast())
    {
        Int_t nHits = fAnaItemsFrs->GetEntriesFast();
        // std::cout << nHits << std::endl;

        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BFrsData* hit = (R3BFrsData*)fAnaItemsFrs->At(ihit);
            if (!hit)
                continue;
            fh_Frs_Z->Fill(hit->GetZ());
            fh_Frs_Aq->Fill(hit->GetAq());
            fh_Frs_ID->Fill(hit->GetAq(), hit->GetZ());
        }
    }

    fNEvents += 1;
}

void R3BFrsOnlineSpectra::FinishEvent()
{

    if (fMappedItemsFrs)
    {
        fMappedItemsFrs->Clear();
    }
    if (fCalItemsMusic)
    {
        fCalItemsMusic->Clear();
    }
    if (fCalItemsTpc)
    {
        fCalItemsTpc->Clear();
    }
    if (fHitItemsTpc)
    {
        fHitItemsTpc->Clear();
    }
    if (fHitItemsMw)
    {
        fHitItemsMw->Clear();
    }
    if (fAnaItemsFrs)
    {
        fAnaItemsFrs->Clear();
    }
    if (fWRItemsMaster)
    {
        fWRItemsMaster->Clear();
    }
}

void R3BFrsOnlineSpectra::FinishTask()
{

    cTrigger->Write();
    if (fWRItemsMaster)
    {
        cWr->Write();
    }
    if (fMappedItemsFrs)
    {
        cTrigCom->Write();
        cSee->Write();
        cSeeCom->Write();
        cSeet->Write();
        cIc->Write();
        cSci00->Write();
        cSci01->Write();
        cSci02->Write();
        cSCI21->Write();
        cSCI41->Write();
        cSCI42->Write();
        cSCI81->Write();
        cSCI_21_41->Write();
        cSCI_21_81->Write();
        cSCI_tof2141R->Write();
        cSCI_tof2141L->Write();
        cSCI_tof2141->Write();
        cSCI_tof21pos->Write();
        cSCI_tof41pos->Write();
    }
    if (fMapItemsMusic)
    {
        cMus1->Write();
        cMus2->Write();
        cMus3->Write();
        cMu41_mu42->Write();
    }
    if (fCalItemsMusic)
    {
        cMus1cal->Write();
        cMus2cal->Write();
        cMus3cal->Write();
    }
    if (fHitItemsMusic)
    {
        for (Int_t i = 0; i < 3; i++)
            cMushit[i]->Write();
    }
    if (fCalItemsTpc)
    {
        cCalx->Write();
        for (Int_t i = 0; i < 5; i++)
        {
            cTpcCal[i]->Write();
        }
    }
    if (fHitItemsTpc)
    {
        cHitx->Write();
        cHity->Write();
        cHitxy->Write();
        cCalx31->Write();
        cTransS2->Write();
        cTransS4->Write();
        cTrackS2->Write();
        cTrackS4->Write();
    }
    if (fHitItemsMw)
    {
        cMW1->Write();
        cMW2->Write();
    }
    if (fAnaItemsFrs)
    {
        c1ID->Write();
    }
}

ClassImp(R3BFrsOnlineSpectra)
