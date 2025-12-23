#include "Door.h"
#include "HTML.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <vector>
#include <map>
#include <fstream>
#include <string>


bool Door::Create(const CsvRow& row, size_t row_index, std::vector<CsvError>& errors)
{
    auto fatal = [&](const std::string name_, const std::string label_, const std::string& msg)
        {
            std::string error = name_ + " " + label_ + " " + msg;
            errors.push_back({ row_index, error });
            return false;
        };

    CopyCsvText(row, "Name", name, MAXTEXTSIZE);
    CopyCsvText(row, "Cab#", label, MAXTEXTSIZE);
    CopyCsvText(row, "Notes", notes, MAXTEXTSIZE);
    CopyCsvText(row, "Material", material, MAXTEXTSIZE);

    if (!ReadUInt(row, "Count", quantity) || quantity == 0)
        return fatal(name, label, "Invalid or missing Count");

    if (!ReadDouble(row, "Actual Width", dimensions.finishedWidth))
        return fatal(name, label, "Missing or invalid Actual Width");

    if (!ReadDouble(row, "Actual Height", dimensions.finishedHeight))
        return fatal(name, label, "Missing or invalid Actual Height");

    if (!ReadFaceType(row, type))
        return fatal(name, label, "Invalid or missing Type");

    if (!ReadConstruction(row, construction))
        return fatal(name, label, "Invalid or missing Construction");

    // Non-fatal defaults
    ReadDouble(row, "WidthOversize", dimensions.oversizeWidth);
    ReadDouble(row, "HeightOversize", dimensions.oversizeHeight);
    ReadDouble(row, "Rabbet", dimensions.shakerparts.rabbet);
    ReadDouble(row, "Bone Detail", dimensions.bonedetail);

    ReadDouble(row, "Bottom Rail", dimensions.shakerparts.width[static_cast<int>(ShakerPart::BOTTOM_RAIL)]);
    ReadDouble(row, "Top Rail", dimensions.shakerparts.width[static_cast<int>(ShakerPart::TOP_RAIL)]);
    ReadDouble(row, "Left Stile", dimensions.shakerparts.width[static_cast<int>(ShakerPart::LEFT_STILE)]);
    ReadDouble(row, "Right Stile", dimensions.shakerparts.width[static_cast<int>(ShakerPart::RIGHT_STILE)]);
    ReadDouble(row, "Mid Rail/Stile", dimensions.shakerparts.width[static_cast<int>(ShakerPart::MID_RAIL)]);
    ReadDouble(row, "Mid Rail/Stile", dimensions.shakerparts.width[static_cast<int>(ShakerPart::MID_STILE)]);

    ReadDouble(row, "StickTolerance", dimensions.shakerparts.stick_tolerance);
    ReadDouble(row, "CopeTolerance", dimensions.shakerparts.cope_tolerance);

    ReadUInt(row, "Mid Rail Count", dimensions.shakerparts.mid_rail_count);
    ReadUInt(row, "Mid Stile Count", dimensions.shakerparts.mid_stile_count);

    ReadOrientation(row, dimensions.panel.orientation);
    ReadPanel(row, dimensions.panel.hasPanel);

    return true;
}

bool Door::ValidatePanel(double& outWidth, double& outHeight) const
{
	const double minPanelSize = 1.0;
	outWidth = dimensions.panel.GetPanelWidth(construction, dimensions.shakerparts, dimensions.GetOversizedWidth(), dimensions.GetOversizedHeight());
	outHeight = dimensions.panel.GetPanelHeight(construction, dimensions.shakerparts, dimensions.GetOversizedWidth(), dimensions.GetOversizedHeight());
	if (construction == Construction::Shaker || construction == Construction::SmallShaker)
    {
        if (outWidth < minPanelSize || outHeight < minPanelSize)
            return false;
    }
    return true;
}
bool Door::ValidateShakerParts(std::string& error) const
{
    if (construction == Construction::Shaker)
    {
        for (size_t i = 0; i < static_cast<int>(ShakerPart::SHAKERPARTCOUNT); i++)
        {
            if (dimensions.shakerparts.width[i] <= 0.0)
            {
                std::string part = dimensions.shakerparts.GetPartString(static_cast<ShakerPart>(i));
                if (static_cast<ShakerPart>(i) == ShakerPart::MID_RAIL && dimensions.shakerparts.mid_rail_count > 0)
                {
                    error += part + " Width undefined";
                    return false;
                }
                else if (static_cast<ShakerPart>(i) == ShakerPart::MID_STILE && dimensions.shakerparts.mid_stile_count > 0)
                {
                    error += part + " Width undefined";
                    return false;
                }
                else
                {
                    error += part + " Width undefined";
                    return false;
                }
            }
        }
    }
    if (construction == Construction::SmallShaker)
    {
        for (size_t i = 0; i < static_cast<int>(ShakerPart::SHAKERPARTCOUNT); i++)
        {
            if (dimensions.shakerparts.width[i] <= 0.0)
            {
                std::string part = dimensions.shakerparts.GetPartString(static_cast<ShakerPart>(i));
                if (static_cast<ShakerPart>(i) == ShakerPart::MID_RAIL)
                {
                    continue;
                }
                else if (static_cast<ShakerPart>(i) == ShakerPart::MID_STILE)
                {
                    continue;
                }
                else
                {
                    error += part + " Width undefined";
                    return false;
                }
            }
        }
    }
    return true;
}

void Door::Print() const
{
    int denom = 32;
    std::cout << getTypeString() << " \n"
        << getConstructionString() << " \n"
        << getNameString() << "\n"
        << getLabelString() << "\n"
        << getMaterialString() << "\n"
        << getNotesString() << "\n"
        << getQuantityString() << "\n"
        << getFinishedWidth() << "\n"
        << getFinishedHeight() << "\n"
        << getLeftStileWidthString(denom) << " " << getLeftStileLengthString(denom) << "\n"
        << getRightStileWidthString(denom) << " " << getRightStileLengthString(denom) << "\n"
        << getTopRailWidthString(denom) << " " << getTopRailLengthString(denom) << "\n"
        << getBottomRailWidthString(denom) << " " << getBottomRailLengthString(denom) << "\n";
    if (getMidRailcount() > 0)
        std::cout << getMidRailWidthString(denom) << " " << getMidRailLengthString(denom) << " Rail count: " << getMidRailcount() << "\n";
    if (getMidStilecount() > 0)
        std::cout << getMidRailWidthString(denom) << " " << getMidStileLengthString(denom) << " Stile count: " << getMidStilecount() << "\n";

    std::cout << getPanelWidthString(denom) << "\n"
    << getPanelHeightString(denom) << "\n"
    << "Panel Count: " << getPanelcount() << "\n" << std::endl;
}

void Door::AppendTigerStopCuts(std::vector<TigerStopItem>& cutlist) const
{
    const double w = dimensions.GetOversizedWidth();
    const double h = dimensions.GetOversizedHeight();
    const auto& parts = dimensions.shakerparts;

    auto add = [&](ShakerPart part, unsigned int qty)
        {
            if (qty == 0)
                return;

            TigerStopItem c{};
            if (getConstruction() == Construction::Shaker)
			    c.group = GetStockGroup(part);
            if (getConstruction() == Construction::SmallShaker)
                c.group = StockGroup::Small_Shaker_Rail;
			c.material = std::string(material);
            c.length = parts.GetCutLength(construction, part, w, h);
            c.quantity = qty;
            c.nominal_width = parts.width[static_cast<int>(part)];
            cutlist.push_back(c);
        };

    if (getConstruction() == Construction::Shaker || getConstruction() == Construction::SmallShaker)
    {
        add(ShakerPart::TOP_RAIL, quantity);
        add(ShakerPart::BOTTOM_RAIL, quantity);
        add(ShakerPart::LEFT_STILE, quantity);
        add(ShakerPart::RIGHT_STILE, quantity);
        add(ShakerPart::MID_RAIL, quantity * parts.mid_rail_count);
        add(ShakerPart::MID_STILE, quantity * parts.mid_stile_count);
    }
}

void DoorList::ReadCsvTable(CsvTable doorsTable)
{
    std::vector<CsvError> errors;
    unsigned int skippedCount = 0;
    for (size_t i = 0; i < doorsTable.rows.size(); ++i)
    {
        Door d;
        if (d.Create(doorsTable.rows[i], i + 2, errors)) // +2 for header row
            m_doors.push_back(d);
        else
            skippedCount++;

    }
    std::cout << "Skipped " << skippedCount << " doors\n";
    for (const auto& e : errors)
    {
        std::cout << "CSV Row " << e.row_index << " skipped: " << e.message << "\n";
    }
    std::cout << "\n";
    std::erase_if(m_doors, [] (const Door& door) {
        double panelWidth = 0.0;
        double panelHeight = 0.0;

        bool isValid = door.ValidatePanel(panelWidth, panelHeight);

        if (!isValid)
        {
            std::cout << "Warning: Removed Door "
                << door.getNameString() << " " << door.getLabelString()
                << " Below minimum panel size. "
                << panelWidth << " x " << panelHeight << "\n";
        }
        else
        {
            std::string errormsg = "";
            isValid = door.ValidateShakerParts(errormsg);

            if (!isValid)
            {
                std::cout << "Warning: Removed Door "
                    << door.getNameString() << " " << door.getLabelString()
                    << " " << errormsg << "\n";
            }
        }

        return !isValid;
        });

    std::cout << "\nProcessed " << m_doors.size() << " valid door(s)\n";
    makeUniqueLabels();
}

void DoorList::WriteHTMLReport(const char* jobname) const
{
    constexpr int denom = 32;
	std::string title = std::string(jobname) + " Door Report";
    std::string file = std::string(jobname) + " Door Report.html";
    Html::HtmlDocument doc(title);




    doc.AddStyle(R"(

/* =====================
   NORMAL LAYOUT
===================== */

.door-row {
    display: flex;
    width: 100%;
    gap: 1px;
    align-items: center;
    margin-bottom: 0px;
}

.door-data { 
    flex: 0 0 80%;
    gap: 1px;
 }

.door-drawing {
    flex: 0 0 20%;
    display: flex;
    align-items: center;
    justify-content: center;
    align-self: flex-start;
}


.door-drawing svg {
    width: 98%;
    height: auto;
    max-height: 1.1in;   /* tune to taste */
    display: block;
}

.door-grid {
    display: grid;
    grid-template-columns: 1fr;
    gap: 1px;
    display: block;
}

table {
    border-collapse: collapse;  
}
.door-block {
    border: 1px solid #000;
    background-color: #ffffff;
    margin-bottom: 0px;
    padding: 0px;
    border-radius: 3px;
}

.door-block {
    margin-top: 0;
    margin-bottom: 0;
}

h1,h2,h3,h4,h5,h6 {
    white-space: pre; 
    margin: 0;
}

p {
    margin: 0;
}
td {
    padding: 0;
    margin: 0;
    white-space: pre;   /* VERY IMPORTANT */
}

/* =====================
   PAGE + PRINT CONTROL
===================== */

@page {
    size: Letter;
    margin: 0.125in;
}

thead { display: table-header-group; }
tfoot { display: table-footer-group; }


.header-space,
.footer-space {
    height: 0.1in;        /* adjust to taste */
}

/* ===== FIXED PRINT HEADER ===== */
.page-header {
    position: fixed;
    top: 0.1in;
    left: 0.25in;
    right: 0.25in;

    font-family: Arial, sans-serif;
    font-size: 12px;
    padding: 4px 0;
}

/* Optional footer (if you want later)
.page-footer {
    position: fixed;
    bottom: 0.25in;
    left: 0.25in;
    right: 0.25in;
}
*/

/* screen preview nice */
@media screen {
    body { background:#ccc; }

    .page {
        width: 8.5in;
        margin: 0 auto;
        background: white;
        box-shadow: 0 0 5px rgba(0,0,0,0.4);
    }
}
.door-block {
   break-inside: avoid;
   page-break-inside: avoid;
}

)");


    doc.AddRawHtml(R"(
<table class="page-table">

<thead>
<tr><td>
    <div class="header-space"></div>
</td></tr>
</thead>

<tbody>
<tr><td>
)");


    doc.BeginGrid("door-grid");

    for (const auto& door : m_doors)
    {
        doc.AddRawHtml("<div class='door-block'>");
        doc.AddRawHtml("<div class='door-row'>");
        doc.AddRawHtml("<div class='door-data'>");


        unsigned int quantity = door.getQuantity();
        std::string spacer = "  |  ";

        std::string header = std::string(door.getConstructionString()) + " " + std::string(door.getTypeString()) + " " + door.getNameString() + spacer + door.getLabelString() + spacer + door.getGrainOrientationString() + 
            "\n" + door.getMaterialString() + spacer + door.getQuantityString();
        if (door.hasBoneDetail())
            header += spacer + door.getBoneDetailString(denom);
        if (door.hasNotes())
			header += spacer + std::string(door.getNotes());
        doc.AddHeading(header, 3);


        std::string finishedwidth = door.getFinishedWidthString(denom);
        std::string cutwidth = door.getCutWidthString(denom) + "\n" + door.getOversizeWidthString(denom);
		std::string finishedheight = door.getFinishedHeightString(denom);
		std::string cutheight = door.getCutHeightString(denom) + "\n" + door.getOversizeHeightString(denom);
		std::string panelwidth = door.getPanelWidthString(denom);
		std::string panelheight = door.getPanelHeightString(denom);
        Html::HtmlTable maintable;

        if (door.hasPanel())
        {
            maintable.AddColumn({ "", "33%" });
            maintable.AddColumn({ "", "33%" });
            maintable.AddColumn({ "", "33%" });

            maintable.AddRow({ finishedwidth, cutwidth, panelwidth });
		    maintable.AddRow({ finishedheight, cutheight, panelheight });
        }
        else
        {
            maintable.AddColumn({ "", "50%" });
            maintable.AddColumn({ "", "50%" });

            maintable.AddRow({ finishedwidth, cutwidth });
            maintable.AddRow({ finishedheight, cutheight });
        }

        Html::HtmlTable shakerTable;
		shakerTable.AddColumn({ door.getLeftStileWidthString(denom), "16.6%" });
        shakerTable.AddColumn({ door.getRightStileWidthString(denom), "16.6%" });
        shakerTable.AddColumn({ door.getTopRailWidthString(denom), "16.6%" });
        shakerTable.AddColumn({ door.getBottomRailWidthString(denom), "16.6%" });
		if (door.hasMidRail())
            shakerTable.AddColumn({ door.getMidRailWidthString(denom), "16.6%" });
		if (door.hasMidStile())
            shakerTable.AddColumn({ door.getMidStileWidthString(denom), "16.6%" });

        if (door.hasMidRail() && door.hasMidStile())
        {
            shakerTable.AddRow({ door.getLeftStileLengthString(denom),
                door.getRightStileLengthString(denom),
                door.getTopRailLengthString(denom),
                door.getBottomRailLengthString(denom),
                door.getMidRailLengthString(denom),
                door.getMidStileLengthString(denom) });
        }
        else if (door.hasMidRail() && !door.hasMidStile())
        {
            shakerTable.AddRow({ door.getLeftStileLengthString(denom),
                door.getRightStileLengthString(denom),
                door.getTopRailLengthString(denom),
                door.getBottomRailLengthString(denom),
                door.getMidRailLengthString(denom) });
        }
        else if (!door.hasMidRail() && door.hasMidStile())
        {
            shakerTable.AddRow({ door.getLeftStileLengthString(denom),
                door.getRightStileLengthString(denom),
                door.getTopRailLengthString(denom),
                door.getBottomRailLengthString(denom),
                door.getMidStileLengthString(denom) });
        }
        else
        {
            shakerTable.AddRow({ door.getLeftStileLengthString(denom),
                door.getRightStileLengthString(denom),
                door.getTopRailLengthString(denom),
                door.getBottomRailLengthString(denom) });
        }
        
        doc.AddTable(maintable);
        if (door.getConstruction() == Construction::Shaker || door.getConstruction() == Construction::SmallShaker)
        {
		    doc.AddTable(shakerTable);
        }
        doc.AddRawHtml("</div>");

        doc.AddRawHtml("<div class='door-drawing'>");
        Html::Svg::DoorDiagram diagram;

		Html::Svg::DoorStyle style = Html::Svg::DoorStyle::Slab;
		if (door.getConstruction() == Construction::Shaker)
			style = Html::Svg::DoorStyle::Shaker;
		if (door.getConstruction() == Construction::SmallShaker)
			style = Html::Svg::DoorStyle::ShakerMitered;

        diagram
            .SetSize(50, 50)             // CSS size
            .SetViewBox(0, 0, door.getFinishedWidth(), door.getFinishedHeight())     // logical drawing space
            .SetDoorStyle(style)
            .SetLeftStileWidth(door.GetShakerPartWidth(ShakerPart::LEFT_STILE))
            .SetRightStileWidth(door.GetShakerPartWidth(ShakerPart::RIGHT_STILE))
            .SetTopRailWidth(door.GetShakerPartWidth(ShakerPart::TOP_RAIL))
            .SetBottomRailWidth(door.GetShakerPartWidth(ShakerPart::BOTTOM_RAIL))
            .SetMidWidth(door.GetShakerPartWidth(ShakerPart::MID_RAIL))
            .SetMidRail(door.getMidRailcount())
			.SetMidStile(door.getMidStilecount())
            .SetBoneDetail(door.GetBoneDetail())
            .SetStrokeWidth(0.1)
            .SetLabel(door.getsvgLabel());

        doc.AddRawHtml(diagram.ToHtml()); // or Slab / Mitered
        doc.AddRawHtml("</div>");
        doc.AddRawHtml("</div>");
        doc.AddRawHtml("</div>");
    }

    
    doc.EndGrid();

    doc.AddRawHtml(R"( 
</td></tr>
</tbody>

<tfoot>
<tr><td>
    <div class="footer-space"></div>
</td></tr>
</tfoot>

</table>
)");

    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);

    std::tm tm{};
    localtime_s(&tm, &t);   // <-- safe MSVC version

    std::ostringstream hdr;
    hdr << "<div class='page-header'>Job: "
        << jobname
        << " &nbsp;&nbsp; | &nbsp;&nbsp; Date: "
        << std::put_time(&tm, "%Y-%m-%d")
        << "</div>";

    doc.AddRawHtml(hdr.str());


    doc.WriteToFile(file);
}

void DoorList::WritePanelCsvs(std::string& jobname) const
{
    if (containsShaker())
    {
        for (const auto& door : m_doors)
        {
            if (door.getConstruction() != Construction::Shaker)
                continue;
            if (!door.hasPanel())
                continue;
            const std::string material = door.GetPanelMaterial();
            std::filesystem::path dir(material);
            std::filesystem::create_directories(dir);

            std::ostringstream filename;
            filename << jobname << " " << material << " Shaker Panels.csv";

            std::filesystem::path filePath = dir / filename.str();

            std::ofstream out(filePath, std::ios::app);

            if (std::filesystem::file_size(filePath) == 0)
            {
                out << "Name,Label,Qty,Width,Height,Rabbet\n";
            }

            out << door.getPanelName() << ","
                << door.getPanelLabel() << ","
                << door.getPanelQuantity() << ","
                << FormatTrimmed(door.GetPanelWidth()) << ","
                << FormatTrimmed(door.GetPanelHeight()) << ","
                << FormatTrimmed(door.GetPanelRabbet()) << "\n";
        }
    }
    if (containsSmallShaker())
    {
        for (const auto& door : m_doors)
        {
            if (door.getConstruction() != Construction::SmallShaker)
                continue;

            const std::string material = door.GetPanelMaterial();
            std::filesystem::path dir(material);
            std::filesystem::create_directories(dir);

            std::ostringstream filename;
            filename << jobname << " " << material << " Small Shaker Panels.csv";

            std::filesystem::path filePath = dir / filename.str();

            std::ofstream out(filePath, std::ios::app);

            if (std::filesystem::file_size(filePath) == 0)
            {
                out << "Name,Label,Qty,Width,Height\n";
            }

            out << door.getPanelName() << ","
                << door.getPanelLabel() << ","
                << door.getPanelQuantity() << ","
                << FormatTrimmed(door.GetPanelWidth()) << ","
                << FormatTrimmed(door.GetPanelHeight()) << "\n";
        }
    }
    if (containsSlab())
    {
        for (const auto& door : m_doors)
        {
            if (door.getConstruction() != Construction::Slab)
                continue;

            const std::string material = door.GetPanelMaterial();
            std::filesystem::path dir(material);
            std::filesystem::create_directories(dir);

            std::ostringstream filename;
            filename << jobname << " " << material << " Slab Doors.csv";

            std::filesystem::path filePath = dir / filename.str();

            std::ofstream out(filePath, std::ios::app);

            if (std::filesystem::file_size(filePath) == 0)
            {
                out << "Name,Label,Qty,Width,Height\n";
            }

            out << door.getPanelName() << ","
                << door.getPanelLabel() << ","
                << door.getPanelQuantity() << ","
                << FormatTrimmed(door.GetPanelWidth()) << ","
                << FormatTrimmed(door.GetPanelHeight()) << "\n";
        }
    }
}


// helper to turn group enum into text
static std::string GroupToString(StockGroup g)
{
    switch (g)
    {
    case StockGroup::Rail: return "Rails";
    case StockGroup::Stile: return "Stiles";
    case StockGroup::Small_Shaker_Rail: return "Small Shaker";
    }
    return "UNKNOWN";
}

void WriteGroupedCSVs(const std::vector<TigerStopItem>& items,
    const std::string& jobname)
{
    using LengthMap = std::map<double, unsigned int, std::greater<double>>;
	using Material = std::string;
    // Material ? Group ? Width ? Lengths
    using WidthMap = std::map<double, LengthMap>;
    using GroupMap = std::map<StockGroup, WidthMap>;
    using MaterialMap = std::map<Material, GroupMap>;

    MaterialMap grouped;

    // ---------- Grouping ----------
    for (const auto& it : items)
    {
        grouped[it.material]
            [it.group]
            [it.nominal_width]
            [it.length] += it.quantity;
    }

    std::filesystem::path dir("Tiger Stop");
    std::filesystem::create_directories(dir);

    // ---------- Writing ----------
    for (auto& [material, groups] : grouped)
    {
        for (auto& [group, widths] : groups)
        {
            for (auto& [width, lengths] : widths)
            {
                std::ostringstream filename;
                filename << jobname << " "
                    << material << " "
                    << GroupToString(group) << " "
                    << FormatTrimmed(width)
                    << ".csv";

                std::ofstream out(dir / filename.str());
                if (!out)
                    continue;

                out << "length,quantity\n";

                for (auto& [length, qty] : lengths)
                {
                    out << FormatTrimmed(length) << ","
                        << qty << "\n";
                }
            }
        }
    }
}


void DoorList::WriteTigerStopCsvs(std::string& jobname) const
{
    std::vector<TigerStopItem> cutlist;

    for (const auto& door : m_doors)
    {
        if (door.getConstruction() == Construction::Shaker || door.getConstruction() == Construction::SmallShaker)
            door.AppendTigerStopCuts(cutlist);
    }
    WriteGroupedCSVs(cutlist, jobname);
}

DoorList::DoorList(CsvTable doorsTable)
{
    ReadCsvTable(doorsTable);
}

void DoorList::Print()
{
    for (const auto& door : m_doors)
        door.Print();
}

void DoorList::makeUniqueLabels()
{
    std::unordered_map<std::string, int> totalCount;

    // ---- PASS 1: count frequencies ----
    for (auto& d : m_doors)
        ++totalCount[d.getlabelPtr()];

    std::unordered_map<std::string, int> seen;

    // ---- PASS 2: assign suffixes if needed ----
    for (auto& d : m_doors)
    {
        std::string base = d.getlabelPtr();

        if (totalCount[base] > 1)
        {
            int idx = seen[base]++;

            // convert idx -> A,B,C,... AA,AB...
            int n = idx;
            std::string suffix;

            do
            {
                suffix.insert(suffix.begin(), char('A' + (n % 26)));
                n = n / 26 - 1;
            } while (n >= 0);

            std::snprintf(d.getlabelPtr(), MAXTEXTSIZE, "%s%s",
                base.c_str(), suffix.c_str());
        }
    }
}
