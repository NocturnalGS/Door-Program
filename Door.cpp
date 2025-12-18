#include <iostream>
#include <format>
#include "Door.h"
#include "CsvUtils.h"
#include "TigerStop.h"

//void Door::Create(const CsvRow& row)
//{
//    CopyCsvText(row, "Name", name, MAXTEXTSIZE);
//    CopyCsvText(row, "Cab#", label, MAXTEXTSIZE);
//    CopyCsvText(row, "Notes", notes, MAXTEXTSIZE);
//
//    if (!ReadInt(row, "Count", quantity))
//        quantity = 1;
//
//    ReadDouble(row, "Actual Width", dimensions.finishedWidth);
//    ReadDouble(row, "Actual Height", dimensions.finishedHeight);
//    ReadDouble(row, "WidthOversize", dimensions.oversizeWidth);
//    ReadDouble(row, "HeightOversize", dimensions.oversizeHeight);
//    ReadDouble(row, "Rabbet", dimensions.shakerparts.rabbet);
//    ReadDouble(row, "Bottom Rail Width", dimensions.shakerparts.width[BOTTOM_RAIL]);
//    ReadDouble(row, "Top Rail Width", dimensions.shakerparts.width[TOP_RAIL]);
//    ReadDouble(row, "Left Stile Width", dimensions.shakerparts.width[LEFT_STILE]);
//    ReadDouble(row, "Right Stile Width", dimensions.shakerparts.width[RIGHT_STILE]);
//    ReadDouble(row, "Mid Rail/Stile Width", dimensions.shakerparts.width[MID_RAIL]);
//    ReadDouble(row, "Mid Rail/Stile Width", dimensions.shakerparts.width[MID_STILE]);
//    ReadDouble(row, "StickTolerance", dimensions.shakerparts.stick_tolerance);
//    ReadDouble(row, "CopeTolerance", dimensions.shakerparts.cope_tolerance);
//    if (!ReadInt(row, "Mid Rail Count", dimensions.shakerparts.mid_rail_count))
//        dimensions.shakerparts.mid_rail_count = 0;
//    if (!ReadInt(row, "Mid Stile Count", dimensions.shakerparts.mid_stile_count))
//        dimensions.shakerparts.mid_stile_count = 0;
//    ReadFaceType(row, type);
//    ReadConstruction(row, construction);
//    ReadOrientation(row, dimensions.panel.orientation);
//    ReadPanel(row, dimensions.panel.hasPanel);
//}

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

    if (!ReadInt(row, "Count", quantity) || quantity == 0)
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

    ReadDouble(row, "Bottom Rail Width", dimensions.shakerparts.width[BOTTOM_RAIL]);
    ReadDouble(row, "Top Rail Width", dimensions.shakerparts.width[TOP_RAIL]);
    ReadDouble(row, "Left Stile Width", dimensions.shakerparts.width[LEFT_STILE]);
    ReadDouble(row, "Right Stile Width", dimensions.shakerparts.width[RIGHT_STILE]);
    ReadDouble(row, "Mid Rail/Stile Width", dimensions.shakerparts.width[MID_RAIL]);
    ReadDouble(row, "Mid Rail/Stile Width", dimensions.shakerparts.width[MID_STILE]);

    ReadDouble(row, "StickTolerance", dimensions.shakerparts.stick_tolerance);
    ReadDouble(row, "CopeTolerance", dimensions.shakerparts.cope_tolerance);

    ReadInt(row, "Mid Rail Count", dimensions.shakerparts.mid_rail_count);
    ReadInt(row, "Mid Stile Count", dimensions.shakerparts.mid_stile_count);

    ReadOrientation(row, dimensions.panel.orientation);
    ReadPanel(row, dimensions.panel.hasPanel);

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
    case TOP_RAIL:
        width = dimensions.shakerparts.GetPartWidth(TOP_RAIL);
        length = dimensions.shakerparts.GetCutLength(construction, TOP_RAIL, door_width, door_height);
        std::cout << " Top Rail Width: " << width << " Length: " << length << "\n";
        break;
    case BOTTOM_RAIL:
        width = dimensions.shakerparts.GetPartWidth(BOTTOM_RAIL);
        length = dimensions.shakerparts.GetCutLength(construction, BOTTOM_RAIL, door_width, door_height);
        std::cout << " Bottom Rail Width: " << width << " Length: " << length << "\n";
        break;
    case MID_RAIL:
        width = dimensions.shakerparts.GetPartWidth(MID_RAIL);
        length = dimensions.shakerparts.GetCutLength(construction, MID_RAIL, door_width, door_height);
        std::cout << " Mid Rail Width: " << width << " Length: " << length << "\n";
        break;
    case LEFT_STILE:
        width = dimensions.shakerparts.GetPartWidth(LEFT_STILE);
        length = dimensions.shakerparts.GetCutLength(construction, LEFT_STILE, door_width, door_height);
        std::cout << " Left Stile Width: " << width << " Length: " << length << "\n";
        break;
    case RIGHT_STILE:
        width = dimensions.shakerparts.GetPartWidth(RIGHT_STILE);
        length = dimensions.shakerparts.GetCutLength(construction, RIGHT_STILE, door_width, door_height);
        std::cout << " Right Stile Width: " << width << " Length: " << length << "\n";
        break;
    case MID_STILE:
        width = dimensions.shakerparts.GetPartWidth(MID_STILE);
        length = dimensions.shakerparts.GetCutLength(construction, MID_STILE, door_width, door_height);
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
    std::cout << " Notes: " << notes << "\n";
    std::cout << " Count: " << quantity << "\n";
    std::cout << " Finished Width: " << dimensions.finishedWidth << " ";
    std::cout << " Finished Height: " << dimensions.finishedHeight << "\n";
    std::cout << " Oversize Width: " << dimensions.oversizeWidth << " ";
    std::cout << " Oversize Height: " << dimensions.oversizeWidth << "\n";
    PrintPart(TOP_RAIL);
    PrintPart(BOTTOM_RAIL);
    PrintPart(LEFT_STILE);
    PrintPart(RIGHT_STILE);
    if (dimensions.shakerparts.mid_rail_count>0)
        PrintPart(MID_RAIL);
    if (dimensions.shakerparts.mid_stile_count > 0)
        PrintPart(MID_STILE);
    std::cout << " Panel Width " << dimensions.panel.GetPanelWidth(dimensions.shakerparts, dimensions.GetOversizedWidth(), dimensions.GetOversizedHeight()) << "\n";
    std::cout << " Panel Height " << dimensions.panel.GetPanelHeight(dimensions.shakerparts, dimensions.GetOversizedWidth(), dimensions.GetOversizedHeight()) << "\n";
    std::cout << " Panel Count " << dimensions.panel.GetPanelCount(dimensions.shakerparts) << "\n";
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

void Door::AppendTigerStopCuts(std::vector<TigerStopCut>& cuts) const
{
    const double w = dimensions.GetOversizedWidth();
    const double h = dimensions.GetOversizedHeight();
    const auto& parts = dimensions.shakerparts;

    auto add = [&](ShakerPart part, unsigned int qty)
        {
            if (qty == 0)
                return;

            TigerStopCut c{};
            c.group = GetStockGroup(part);
            c.nominal_width = parts.width[part];   // IMPORTANT
            c.length = parts.GetCutLength(construction, part, w, h);
            c.quantity = qty;

            cuts.push_back(c);
        };

    if (getConstruction() == Construction::Shaker)
    {
        add(TOP_RAIL, quantity);
        add(BOTTOM_RAIL, quantity);
        add(LEFT_STILE, quantity);
        add(RIGHT_STILE, quantity);
        add(MID_RAIL, quantity * parts.mid_rail_count);
        add(MID_STILE, quantity * parts.mid_stile_count);
    }
    if (getConstruction() == Construction::Shaker)
    {
        add(TOP_RAIL, quantity);
        add(BOTTOM_RAIL, quantity);
        add(LEFT_STILE, quantity);
        add(RIGHT_STILE, quantity);
        add(MID_RAIL, quantity * parts.mid_rail_count);
        add(MID_STILE, quantity * parts.mid_stile_count);
    }

}

void DoorList::AggregateTigerCuts()
{
    for (const auto& door : m_doors)
    {
        if (door.getConstruction() == Construction::Shaker)
            door.AppendTigerStopCuts(m_shakerTigerCuts);
        if (door.getConstruction() == Construction::SmallShaker)
            door.AppendTigerStopCuts(m_smallShakerTigerCuts);
    }
}





void DoorList::ReadCsvTable(CsvTable doorsTable)
{
    //for (size_t r = 0; r < doorsTable.rows.size(); ++r)
    //{
    //    Door door;
    //    m_doors.push_back(door);
    //    m_doors.back().Create(doorsTable.rows[r]);
    //}
    unsigned int skippedCount = 0;
    for (size_t i = 0; i < doorsTable.rows.size(); ++i)
    {
        Door d;
        if (d.Create(doorsTable.rows[i], i + 2, m_errors)) // +2 for header row
            m_doors.push_back(d);
        else
            skippedCount++;

    }
    //for (const auto& door : m_doors)
    //    door.Print();
    std::cout << "Skipped " << skippedCount << " doors\n";
    for (const auto& e : m_errors)
    {
        std::cout << "Row " << e.row_index << " skipped: " << e.message << "\n";
    }

    std::cout << "\nProcessed " << m_doors.size() << " valid door(s)\n";
}

void DoorList::WriteTigerStopCsvs(const char* folder) const
{
    std::vector<TigerStopCut> cuts;

    for (const auto& door : m_doors)
        door.AppendTigerStopCuts(cuts);

    for (const auto& door : m_doors)
        door.Print();

    //AggregateTigerStopCuts(cuts);
    //WriteTigerStopCsvs(folder, cuts);
}

