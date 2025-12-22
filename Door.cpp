#include "Door.h"
#include "HTML.h"

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

    ReadDouble(row, "Bottom Rail Width", dimensions.shakerparts.width[static_cast<int>(ShakerPart::BOTTOM_RAIL)]);
    ReadDouble(row, "Top Rail Width", dimensions.shakerparts.width[static_cast<int>(ShakerPart::TOP_RAIL)]);
    ReadDouble(row, "Left Stile Width", dimensions.shakerparts.width[static_cast<int>(ShakerPart::LEFT_STILE)]);
    ReadDouble(row, "Right Stile Width", dimensions.shakerparts.width[static_cast<int>(ShakerPart::RIGHT_STILE)]);
    ReadDouble(row, "Mid Rail/Stile Width", dimensions.shakerparts.width[static_cast<int>(ShakerPart::MID_RAIL)]);
    ReadDouble(row, "Mid Rail/Stile Width", dimensions.shakerparts.width[static_cast<int>(ShakerPart::MID_STILE)]);

    ReadDouble(row, "StickTolerance", dimensions.shakerparts.stick_tolerance);
    ReadDouble(row, "CopeTolerance", dimensions.shakerparts.cope_tolerance);

    ReadUInt(row, "Mid Rail Count", dimensions.shakerparts.mid_rail_count);
    ReadUInt(row, "Mid Stile Count", dimensions.shakerparts.mid_stile_count);

    ReadOrientation(row, dimensions.panel.orientation);
    ReadPanel(row, dimensions.panel.hasPanel);

    return true;
}

bool Door::Validate(double& outWidth, double& outHeight) const
{
	const double minPanelSize = 1.0;
	outWidth = dimensions.panel.GetInnerPanelWidth(construction, dimensions.shakerparts, dimensions.GetOversizedWidth(), dimensions.GetOversizedHeight());
	outHeight = dimensions.panel.GetInnerPanelHeight(construction, dimensions.shakerparts, dimensions.GetOversizedWidth(), dimensions.GetOversizedHeight());
	if (construction == Construction::Shaker || construction == Construction::SmallShaker)
    {
        if (outWidth < minPanelSize || outHeight < minPanelSize)
            return false;
    }
    return true;
}

void Door::PrintPart(ShakerPart part) const
{
    double door_width = dimensions.GetOversizedWidth();
    double door_height = dimensions.GetOversizedHeight();
    double width = 0.0;
    double length = 0.0;
    switch (part)
    {
    case ShakerPart::TOP_RAIL:
        width = dimensions.shakerparts.GetPartWidth(ShakerPart::TOP_RAIL);
        length = dimensions.shakerparts.GetCutLength(construction, ShakerPart::TOP_RAIL, door_width, door_height);
        std::cout << " Top Rail Width: " << width << " Length: " << length << "\n";
        break;
    case ShakerPart::BOTTOM_RAIL:
        width = dimensions.shakerparts.GetPartWidth(ShakerPart::BOTTOM_RAIL);
        length = dimensions.shakerparts.GetCutLength(construction, ShakerPart::BOTTOM_RAIL, door_width, door_height);
        std::cout << " Bottom Rail Width: " << width << " Length: " << length << "\n";
        break;
    case ShakerPart::MID_RAIL:
        width = dimensions.shakerparts.GetPartWidth(ShakerPart::MID_RAIL);
        length = dimensions.shakerparts.GetCutLength(construction, ShakerPart::MID_RAIL, door_width, door_height);
        std::cout << " Mid Rail Width: " << width << " Length: " << length << "\n";
        break;
    case ShakerPart::LEFT_STILE:
        width = dimensions.shakerparts.GetPartWidth(ShakerPart::LEFT_STILE);
        length = dimensions.shakerparts.GetCutLength(construction, ShakerPart::LEFT_STILE, door_width, door_height);
        std::cout << " Left Stile Width: " << width << " Length: " << length << "\n";
        break;
    case ShakerPart::RIGHT_STILE:
        width = dimensions.shakerparts.GetPartWidth(ShakerPart::RIGHT_STILE);
        length = dimensions.shakerparts.GetCutLength(construction, ShakerPart::RIGHT_STILE, door_width, door_height);
        std::cout << " Right Stile Width: " << width << " Length: " << length << "\n";
        break;
    case ShakerPart::MID_STILE:
        width = dimensions.shakerparts.GetPartWidth(ShakerPart::MID_STILE);
        length = dimensions.shakerparts.GetCutLength(construction, ShakerPart::MID_STILE, door_width, door_height);
        std::cout << " Mid Stile Width: " << width << " Length: " << length << "\n";
        break;
    }
}

void Door::Print() const
{
    PrintType();
    PrintConstruction();
    std::cout << " Name: " << name << "\n";
    std::cout << " Cab#: " << label << "\n";
    std::cout << " Material: " << material << "\n";
    std::cout << " Notes: " << notes << "\n";
    std::cout << " Count: " << quantity << "\n";
    std::cout << " Finished Width: " << dimensions.finishedWidth << " ";
    std::cout << " Finished Height: " << dimensions.finishedHeight << "\n";
    std::cout << " Oversize Width: " << dimensions.oversizeWidth << " ";
    std::cout << " Oversize Height: " << dimensions.oversizeWidth << "\n";
    PrintPart(ShakerPart::TOP_RAIL);
    PrintPart(ShakerPart::BOTTOM_RAIL);
    PrintPart(ShakerPart::LEFT_STILE);
    PrintPart(ShakerPart::RIGHT_STILE);
    if (dimensions.shakerparts.mid_rail_count>0)
        PrintPart(ShakerPart::MID_RAIL);
    if (dimensions.shakerparts.mid_stile_count > 0)
        PrintPart(ShakerPart::MID_STILE);
    std::cout << " Panel Width " << dimensions.panel.GetPanelWidthWithRabbet(construction, dimensions.shakerparts, dimensions.GetOversizedWidth(), dimensions.GetOversizedHeight()) << "\n";
    std::cout << " Panel Height " << dimensions.panel.GetPanelHeightWithRabbet(construction, dimensions.shakerparts, dimensions.GetOversizedWidth(), dimensions.GetOversizedHeight()) << "\n";
    std::cout << " Panel Count " << dimensions.panel.GetPanelCount(construction, dimensions.shakerparts) << "\n";
    std::cout << std::endl;
}

void Door::PrintType() const
{
    switch (type)
    {
    case FaceType::Door:
        std::cout << " Type: Door" << "\n";
        break;
    case FaceType::Drawer:
        std::cout << " Type: Drawer" << "\n";
        break;
    case FaceType::Panel:
        std::cout << " Type: Panel" << "\n";
        break;
    }
}
void Door::PrintConstruction() const
{
    switch (construction)
    {
    case Construction::Slab:
        std::cout << " Construction: Slab" << "\n";
        break;
    case Construction::Shaker:
        std::cout << " Construction: Shaker" << "\n";
        break;
    case Construction::SmallShaker:
        std::cout << " Construction: Small Shaker" << "\n";
        break;
    }
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

        bool isValid = door.Validate(panelWidth, panelHeight);

        if (!isValid)
        {
            std::cout << "Warning: Removed Door "
                << door.getNameString() << " " << door.getLabelString()
                << " Below minimum panel size. "
                << panelWidth << " x " << panelHeight << "\n";
        }

        return !isValid;
        });

    std::cout << "\nProcessed " << m_doors.size() << " valid door(s)\n";
}

bool SortByGroupThenWidthThenLength(const TigerStopItem& a, const TigerStopItem& b)
{
    if (a.group != b.group)
        return a.group < b.group;
    if (a.nominal_width != b.nominal_width)
        return a.nominal_width < b.nominal_width;
    return a.length < b.length;
}

void DoorList::WriteHTMLReport(const char* jobname) const
{
    constexpr int denom = 32;
	std::string title = std::string(jobname) + " Door Report";
    std::string file = std::string(jobname) + " Door Report.html";
    Html::HtmlDocument doc(title);

    doc.AddStyle(R"(

    .door-row {
        display: flex;
        width: 100%;
        gap: 6px;
        align-items: stretch;   /* key: same height */
        margin-bottom: 6px;
    }
    
    .door-data {
        flex: 0 0 75%;
    }
    
    .door-drawing {
        flex: 0 0 25%;
        display: flex;
        align-items: center;
        justify-content: center;
    }
    
    .door-drawing svg {
        width: 80%;
        height: auto;
        max-height: 80%;
    }



    .door-grid {
        display: grid;
        grid-template-columns: 1fr;
        gap: 4px;
    }
    .door-block {
        border: 1px solid #000;
        background-color: #ffffff;
        margin-bottom: 4px;
        padding: 1px;
        border-radius: 6px;
    }
    td {
        white-space: pre;  /* preserves spaces and tabs inside table cells */
    }
    h1, h2, h3, h4, h5, h6 {
        white-space: pre; /* preserve spaces and tabs */
    }
    )");

    doc.BeginGrid("door-grid");

    for (const auto& door : m_doors)
    {
        doc.AddRawHtml("<div class='door-block'>");
        doc.AddRawHtml("<div class='door-row'>");
        doc.AddRawHtml("<div class='door-data'>");


        unsigned int quantity = door.getQuantity();
        std::string spacer = "  |  ";

        std::string header = std::string(door.getConstructionString()) + " " + std::string(door.getTypeString()) + " " + door.getNameString() + spacer + door.getLabelString() +
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
			style = Html::Svg::DoorStyle::Shaker;

        diagram
            .SetSize(100, 100)             // CSS size
            .SetViewBox(0, 0, door.getFinishedWidth(), door.getFinishedHeight())     // logical drawing space
            .SetDoorStyle(style)
            .SetRailWidth(2.5)
            .SetStileWidth(2.5)
            .SetMidRail(false)
            .SetStrokeWidth(0.1);

        doc.AddRawHtml(diagram.ToHtml()); // or Slab / Mitered
        doc.AddRawHtml("</div>");
        doc.AddRawHtml("</div>");
        doc.AddRawHtml("</div>");
    }

    
    doc.EndGrid();

    doc.WriteToFile(file);
}

void DoorList::WriteTigerStopCsvs(const char* folder) const
{
    std::vector<TigerStopItem> cutlist;

    for (const auto& door : m_doors)
    {
        if (door.getConstruction() == Construction::Shaker || door.getConstruction() == Construction::SmallShaker)
            door.AppendTigerStopCuts(cutlist);
    }

    //for (const auto& door : m_doors)
    //    door.Print();



    //consolidate
    std::sort(cutlist.begin(), cutlist.end(), SortByGroupThenWidthThenLength);
 
    for (const auto& cut : cutlist)
    {

    }
    //for (const auto& c : cuts)
    //{
    //    bool merged = false;

    //    for (auto& o : out)
    //    {
    //        if (o.group == c.group && std::fabs(o.nominal_width - c.nominal_width) < EPS && std::fabs(o.length - c.length) < EPS)
    //        {
    //            o.quantity += c.quantity;
    //            merged = true;
    //            break;
    //        }
    //    }

    //    if (!merged)
    //        out.push_back(c);
    //}

    //cuts.swap(out);
    
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