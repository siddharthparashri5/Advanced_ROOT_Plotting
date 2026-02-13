#include <TRint.h>
#include "AdvancedPlotGUI.h"
#include "DataReader.h"
#include "PlotTypes.h"
#include "FitUtils.h"
#include "ErrorHandling.h"
#include "FileHandler.h"

#include <TCanvas.h>
#include <TH1.h>
#include <iostream>
#include <string>

int main(int argc, char** argv)
{
    // -----------------------
    // Batch mode
    // -----------------------
    if (argc >= 3 && std::string(argv[1]) == "--batch") {
        ColumnData data;
        if (!DataReader::ReadFile(argv[2], data)) {
            std::cerr << "Failed to read file\n";
            return 1;
        }

        PlotConfig cfg;
        cfg.type = PlotConfig::kTH1D;
        cfg.xColumn = 0;
        cfg.bins = 100;

        TCanvas c("batch", "Batch Plot", 800, 600);
        TH1* h = PlotCreator::CreateTH1D(data, cfg);
        if (h) {
            h->Draw();
            c.SaveAs("batch_output.png");
            c.SaveAs("batch_output.pdf");
        }
        return 0;
    }

    // -----------------------
    // GUI + ROOT Prompt Mode
    // -----------------------
    TRint app("AdvancedPlotApp", &argc, argv);
    new AdvancedPlotGUI(gClient->GetRoot(), 400, 200);  // Constructor shows GUI
    app.Run();
    return 0;
}

