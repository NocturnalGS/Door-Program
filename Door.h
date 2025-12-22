#pragma once
#include "CsvUtils.h"
#include "HTML.h"

//constants
constexpr size_t MAXTEXTSIZE = 32;
constexpr double ALLOWANCE = 0.015625;
constexpr double RABBET_ALLOWANCE = 0.0625;

//struct forward declarations
struct CsvRow;
struct CsvTable;
struct TigerStopItem;

enum class StockGroup;
enum class FaceType;
enum class Construction;
enum class Orientation;
enum class ShakerPart;

//function forward declarations
inline std::string MakeTigerStopFilename(StockGroup group, double width, std::string jobname);
inline std::string FormatTrimmed(double value);
inline StockGroup GetStockGroup(ShakerPart part);

//struct definitions

enum class StockGroup
{
	Rail,
	Stile,
	Small_Shaker_Rail
};



enum class FaceType
{
	Door,
	Drawer,
	Panel
};

enum class Construction
{
	Slab,
	Shaker,
	SmallShaker
};

enum class Orientation
{
	VERTICAL,
	HORIZONTAL
};

enum class ShakerPart
{
	TOP_RAIL,
	BOTTOM_RAIL,
	LEFT_STILE,
	RIGHT_STILE,
	MID_RAIL,
	MID_STILE,
	SHAKERPARTCOUNT
};

struct TigerStopItem
{
	StockGroup group;
	double length;
	unsigned int quantity;
	double nominal_width;
};

struct ShakerParts
{
	double width[static_cast<int>(ShakerPart::SHAKERPARTCOUNT)] = {};
	double rabbet = 0.0;
	double stick_tolerance = 0.0;
	double cope_tolerance = 0.0;
	unsigned int mid_rail_count = 0;
	unsigned int mid_stile_count = 0;
	double GetPartWidth(ShakerPart part) const
	{ 
		switch(part)
		{
		case ShakerPart::TOP_RAIL:
		case ShakerPart::BOTTOM_RAIL:
		case ShakerPart::LEFT_STILE:
		case ShakerPart::RIGHT_STILE:
			return width[static_cast<int>(part)] - stick_tolerance;
		case ShakerPart::MID_RAIL:
		case ShakerPart::MID_STILE:
			return width[static_cast<int>(part)] - (stick_tolerance * 2.0);
		default:
			return 0.0;
		}
	}
	double GetCutLength(Construction construction, ShakerPart part, double doorWidth, double doorHeight) const
	{
		if (construction == Construction::Shaker)
		{
			switch (part)
			{
			case ShakerPart::TOP_RAIL:
			case ShakerPart::BOTTOM_RAIL:
				return doorWidth - GetPartWidth(ShakerPart::LEFT_STILE) - GetPartWidth(ShakerPart::RIGHT_STILE) + (rabbet * 2) + cope_tolerance * 2;
			case ShakerPart::MID_RAIL:
				if (mid_rail_count == 0)
					return 0.0;
				return doorWidth - GetPartWidth(ShakerPart::LEFT_STILE) - GetPartWidth(ShakerPart::RIGHT_STILE) + (rabbet * 2) + cope_tolerance * 2;
			case ShakerPart::LEFT_STILE:
			case ShakerPart::RIGHT_STILE:
				return doorHeight;
			case ShakerPart::MID_STILE:
				if (mid_stile_count == 0)
					return 0.0;
				return doorHeight - GetPartWidth(ShakerPart::TOP_RAIL) - GetPartWidth(ShakerPart::BOTTOM_RAIL) - (GetPartWidth(ShakerPart::MID_RAIL) * mid_rail_count) + (rabbet * 2) + cope_tolerance * 2;
			default:
				return 0.0;
			}
		}
		if (construction == Construction::SmallShaker)
		{
			switch (part)
			{
			case ShakerPart::TOP_RAIL:
			case ShakerPart::BOTTOM_RAIL:
				return doorWidth;
			case ShakerPart::MID_RAIL:
				if (mid_rail_count == 0)
					return 0.0;
				return doorWidth;
			case ShakerPart::LEFT_STILE:
			case ShakerPart::RIGHT_STILE:
				return doorHeight;
			case ShakerPart::MID_STILE:
				if (mid_stile_count == 0)
					return 0.0;
				return doorHeight;
			default:
				return 0.0;
			}
		}
		return 0.0;
	}
};

struct Panel
{
	Orientation orientation = Orientation::VERTICAL;
	bool hasPanel = false;
private:
	double getWidth(Construction cons, ShakerParts parts, double doorWidth) const
	{
		if (cons == Construction::Shaker)
			return doorWidth - parts.GetPartWidth(ShakerPart::LEFT_STILE) - parts.GetPartWidth(ShakerPart::RIGHT_STILE) - parts.GetPartWidth(ShakerPart::MID_STILE) * parts.mid_stile_count - ALLOWANCE;
		if (cons == Construction::SmallShaker)
			return doorWidth - parts.GetPartWidth(ShakerPart::LEFT_STILE) - parts.GetPartWidth(ShakerPart::RIGHT_STILE);
		return doorWidth;
	}
	double getHeight(Construction cons, ShakerParts parts, double doorHeight) const
	{
		if (cons == Construction::Shaker)
			return doorHeight - parts.GetPartWidth(ShakerPart::TOP_RAIL) - parts.GetPartWidth(ShakerPart::BOTTOM_RAIL) - parts.GetPartWidth(ShakerPart::MID_RAIL) * parts.mid_rail_count - ALLOWANCE;
		if (cons == Construction::SmallShaker)
			return doorHeight - parts.GetPartWidth(ShakerPart::TOP_RAIL) - parts.GetPartWidth(ShakerPart::BOTTOM_RAIL);
		return doorHeight;
	}
public:
	double GetInnerPanelWidth(Construction cons, ShakerParts parts, double doorWidth, double doorHeight) const
	{
		if (orientation == Orientation::VERTICAL)
			return getWidth(cons, parts, doorWidth);
		else
			return getHeight(cons, parts, doorHeight);
	}
	double GetInnerPanelHeight(Construction cons, ShakerParts parts, double doorWidth, double doorHeight) const
	{
		if (orientation == Orientation::VERTICAL)
			return getHeight(cons, parts, doorHeight);
		else
			return getWidth(cons, parts, doorWidth);
	}
	double GetPanelWidthWithRabbet(Construction cons, ShakerParts parts, double doorWidth, double doorHeight) const
	{
		return getWidth(cons, parts, doorWidth) + parts.rabbet * 2 - RABBET_ALLOWANCE;
	}
	double GetPanelHeightWithRabbet(Construction cons, ShakerParts parts, double doorWidth, double doorHeight) const
	{
		return getHeight(cons, parts, doorHeight) + parts.rabbet * 2 - RABBET_ALLOWANCE;
	}
	unsigned int GetPanelCount(Construction cons, ShakerParts parts) const
	{
		if (hasPanel && (cons == Construction::Shaker || cons == Construction::SmallShaker))
			return (parts.mid_rail_count + 1) * (parts.mid_stile_count + 1);
		else
			return 0;
	}
};

struct Dimensions
{
	double finishedWidth = 0.0;
	double finishedHeight = 0.0;
	double oversizeWidth = 0.0;
	double oversizeHeight = 0.0;
	double bonedetail = 0.0;
	ShakerParts shakerparts;
	Panel panel;
	double GetOversizedWidth() const { return finishedWidth + oversizeWidth - (bonedetail * 2.0); }
	double GetOversizedHeight() const { return finishedHeight + oversizeHeight - (bonedetail * 2.0); }
	double GetFinishedWidth() const { return finishedWidth; }
	double GetFinishedHeight() const { return finishedHeight; }
};

class Door
{
	char name[MAXTEXTSIZE] = {};
	char label[MAXTEXTSIZE] = {};
	char material[MAXTEXTSIZE] = {};
	char notes[MAXTEXTSIZE] = {};
	Dimensions dimensions = {};
	unsigned int quantity = 0;
	Construction construction = {};
	FaceType type = {};
	void PrintType() const;
	void PrintPart(ShakerPart part) const;
	void PrintConstruction() const;


public:
	bool Validate(double& outWidth, double& outHeight) const;
	std::string getNotes() const { return "SPECIAL NOTES: " + std::string(notes); }
	bool hasNotes() const { return std::string(notes).length() > 0; }
	unsigned int getQuantity() const { return quantity; }
	bool hasMidRail() const { return dimensions.shakerparts.mid_rail_count > 0; }
	bool hasMidStile() const { return dimensions.shakerparts.mid_stile_count > 0; }
	bool hasBoneDetail() const { return dimensions.bonedetail != 0.0; }
	bool hasPanel() const 
	{
		if (construction != Construction::Shaker && construction != Construction::SmallShaker)
			return false;
		return dimensions.panel.hasPanel;
	}
	std::string getMaterialString() const
	{
		return "Material: " + std::string(material);
	}
	std::string getNotesString() const
	{
		return "Notes: " + std::string(notes);
	}
	std::string getLabelString() const
	{
		return "(Label: " + std::string(label) + ")";
	}
	std::string getNameString() const
	{
		return std::string(name);
	}
	std::string getQuantityString() const
	{
		return "Quantity: " + std::to_string(quantity);
	}
	std::string getBoneDetailString(int denom) const
	{
		Fraction bonedetailfrac(dimensions.bonedetail, denom);
		std::string bonedetailstr = "Bone Detail: " + bonedetailfrac.GetDecimalString();
		return bonedetailstr;
	}
	std::string getPanelWidthString(int denom) const
	{
		double panelWidth = dimensions.panel.GetPanelWidthWithRabbet(construction, dimensions.shakerparts, dimensions.GetOversizedWidth(), dimensions.GetOversizedHeight());
		Fraction panelwidthfrac(panelWidth, denom);
		std::string panelwidthstr = "Panel Width: " + panelwidthfrac.GetDecimalString();
		return panelwidthstr;
	}
	std::string getPanelHeightString(int denom) const
	{
		double panelHeight = dimensions.panel.GetPanelHeightWithRabbet(construction, dimensions.shakerparts, dimensions.GetOversizedWidth(), dimensions.GetOversizedHeight());
		Fraction panelheightfrac(panelHeight, denom);
		std::string panelheightstr = "Panel Height: " + panelheightfrac.GetDecimalString();
		return panelheightstr;
	}
	const char* getConstructionString() const
	{
		switch (construction)
		{
		case Construction::Slab:
			return "Slab";
		case Construction::Shaker:
			return "Shaker";
		case Construction::SmallShaker:
			return "Small Shaker";
		default:
			return "Undefined";
		}
	}
	const char* getTypeString() const
	{
		switch (type)
		{
		case FaceType::Door:
			return "Door";
		case FaceType::Drawer:
			return "Drawer";
		case FaceType::Panel:
			return "Panel";
		default:
			return "Undefined";
		}
	}
	double getFinishedWidth() const { return dimensions.finishedWidth; }
	double getFinishedHeight() const { return dimensions.finishedHeight; }
	double getOversizeWidth() const { return dimensions.oversizeWidth; }
	double getOversizeHeight() const { return dimensions.oversizeHeight; }

	std::string getFinishedWidthString(int denom) const
	{
		Fraction finishedwidthfrac(dimensions.finishedWidth, denom);
		std::string finishedwidth = "Finished Width: " + finishedwidthfrac.GetString();
		return finishedwidth;
	}
	std::string getFinishedHeightString(int denom) const
	{
		Fraction finishedheightfrac(dimensions.finishedHeight, denom);
		std::string finishedheight = "Finished Height: " + finishedheightfrac.GetString();
		return finishedheight;
	}
	std::string getOversizeWidthString(int denom) const
	{
		Fraction oversizewidthfrac(dimensions.oversizeWidth, denom);
		std::string sizeStr = "";
		if (dimensions.oversizeWidth > 0.0)
			sizeStr = "Oversize Width: " + oversizewidthfrac.GetDecimalString();
		if (dimensions.oversizeWidth < 0.0)
			sizeStr = "Undersize Width: " + oversizewidthfrac.GetDecimalString();
		return sizeStr;
	}
	std::string getOversizeHeightString(int denom) const
	{
		Fraction oversizeheightfrac(dimensions.oversizeHeight, denom);
		std::string sizeStr = "";
		if (dimensions.oversizeHeight > 0.0)
			sizeStr = "Oversize Height: " + oversizeheightfrac.GetDecimalString();
		if (dimensions.oversizeHeight < 0.0)
			sizeStr = "Undersize Height: " + oversizeheightfrac.GetDecimalString();
		return sizeStr;
	}
	std::string getCutWidthString(int denom) const
	{
		Fraction width(dimensions.GetOversizedWidth(), denom);
		std::string widthstr = "Cut Width: " + width.GetString();
		return widthstr;
	}
	std::string getCutHeightString(int denom) const
	{
		Fraction height(dimensions.GetOversizedHeight(), denom);
		std::string heightstr = "Cut Height: " + height.GetString();
		return heightstr;
	}
	std::string getLeftStileWidthString(int denom) const
	{ 
		Fraction leftstilewidthfrac(dimensions.shakerparts.width[static_cast<int>(ShakerPart::LEFT_STILE)], denom);
		std::string leftstilewidth = "Left Stile " + leftstilewidthfrac.GetDecimalString();
		return leftstilewidth;
	}
	std::string getRightStileWidthString(int denom) const
	{ 
		Fraction rightstilewidthfrac(dimensions.shakerparts.width[static_cast<int>(ShakerPart::RIGHT_STILE)], denom);
		std::string rightstilewidth = "Right Stile " + rightstilewidthfrac.GetDecimalString();
		return rightstilewidth;
	}
	std::string getTopRailWidthString(int denom) const
	{ 
		Fraction toprailwidthfrac(dimensions.shakerparts.width[static_cast<int>(ShakerPart::TOP_RAIL)], denom);
		std::string toprailwidth = "Top Rail " + toprailwidthfrac.GetDecimalString();
		return toprailwidth;
	}
	std::string getBottomRailWidthString(int denom) const
	{ 
		Fraction bottomrailwidthfrac(dimensions.shakerparts.width[static_cast<int>(ShakerPart::BOTTOM_RAIL)], denom);
		std::string bottomrailwidth = "Bottom Rail " + bottomrailwidthfrac.GetDecimalString();
		return bottomrailwidth;
	}
	std::string getMidRailWidthString(int denom) const
	{ 
		if (dimensions.shakerparts.mid_rail_count > 0) 
		{
			Fraction midrailwidthfrac(dimensions.shakerparts.width[static_cast<int>(ShakerPart::MID_RAIL)], denom);
			std::string midrailwidth = "Mid Rail " + midrailwidthfrac.GetDecimalString();
			return midrailwidth;
		} 
		else 
			return "Mid Rail N/A"; 
	}
	std::string getMidStileWidthString(int denom) const
	{ 
		if (dimensions.shakerparts.mid_stile_count > 0) 
		{
			Fraction midstilewidthfrac(dimensions.shakerparts.width[static_cast<int>(ShakerPart::MID_STILE)], denom);
			std::string midstilewidth = "Mid Stile " + midstilewidthfrac.GetDecimalString();
			return midstilewidth;
		} 
		else 
			return "Mid Stile N/A"; 
	}

	std::string getLeftStileLengthString(int denom) const
	{ 
		Fraction lengthfrac(dimensions.shakerparts.GetCutLength(construction, ShakerPart::LEFT_STILE, dimensions.GetOversizedWidth(), dimensions.GetOversizedHeight()), denom);
		std::string length = "Length: " + lengthfrac.GetDecimalString();
		return length;
	}
	std::string getRightStileLengthString(int denom) const
	{ 
		Fraction lengthfrac(dimensions.shakerparts.GetCutLength(construction, ShakerPart::RIGHT_STILE, dimensions.GetOversizedWidth(), dimensions.GetOversizedHeight()), denom);
		std::string length = "Length: " + lengthfrac.GetDecimalString();
		return length;
	}
	std::string getTopRailLengthString(int denom) const
	{ 
		Fraction lengthfrac(dimensions.shakerparts.GetCutLength(construction, ShakerPart::TOP_RAIL, dimensions.GetOversizedWidth(), dimensions.GetOversizedHeight()), denom);
		std::string length = "Length: " + lengthfrac.GetDecimalString();
		return length;
	}
	std::string getBottomRailLengthString(int denom) const
	{ 
		Fraction lengthfrac(dimensions.shakerparts.GetCutLength(construction, ShakerPart::BOTTOM_RAIL, dimensions.GetOversizedWidth(), dimensions.GetOversizedHeight()), denom);
		std::string length = "Length: " + lengthfrac.GetDecimalString();
		return length;
	}
	std::string getMidRailLengthString(int denom) const
	{ 
		if (dimensions.shakerparts.mid_rail_count > 0) 
		{
			Fraction lengthfrac(dimensions.shakerparts.GetCutLength(construction, ShakerPart::MID_RAIL, dimensions.GetOversizedWidth(), dimensions.GetOversizedHeight()), denom);
			std::string length = "Length: " + lengthfrac.GetDecimalString();
			return length;
		} 
		else 
			return "Length: N/A"; 
	}
	std::string getMidStileLengthString(int denom) const
	{ 
		if (dimensions.shakerparts.mid_stile_count > 0) 
		{
			Fraction lengthfrac(dimensions.shakerparts.GetCutLength(construction, ShakerPart::MID_STILE, dimensions.GetOversizedWidth(), dimensions.GetOversizedHeight()), denom);
			std::string length = "Length: " + lengthfrac.GetDecimalString();
			return length;
		} 
		else 
			return "Length: N/A"; 
	}

	Construction getConstruction() const { return construction; }
	bool Create(const CsvRow& row, size_t row_index, std::vector<CsvError>& errors);
	void Print() const;
	void AppendTigerStopCuts(std::vector<TigerStopItem>& cutlist) const;
	double GetPerimeter() const
	{
		return dimensions.finishedWidth * 2.0 + dimensions.finishedHeight * 2;
	}
private:
	inline bool ReadFaceType(const CsvRow& row, FaceType& out)
	{
		const std::string& s = ToUpper(row["Type"]);

		if (s == "DOOR") { out = FaceType::Door; return true; }
		if (s == "DRAWER") { out = FaceType::Drawer;   return true; }
		if (s == "PANEL") { out = FaceType::Panel; return true; }

		return false;
	}

	inline bool ReadConstruction(const CsvRow& row, Construction& out)
	{
		const std::string& s = ToUpper(row["Construction"]);

		if (s == "SLAB") { out = Construction::Slab; return true; }
		if (s == "SHAKER") { out = Construction::Shaker;   return true; }
		if (s == "SMALL_SHAKER") { out = Construction::SmallShaker; return true; }

		return false;
	}

	inline bool ReadOrientation(const CsvRow& row, Orientation& out)
	{
		const std::string& s = ToUpper(row["Grain Direction"]);

		if (s == "VERTICAL") { out = Orientation::VERTICAL; return true; }
		if (s == "HORIZONTAL") { out = Orientation::HORIZONTAL;   return true; }

		return false;
	}

	inline bool ReadPanel(const CsvRow& row, bool& out)
	{
		const std::string& s = ToUpper(row["Panel"]);

		if (s == "YES") { out = true; return true; }
		else out = false;

		return false;
	}
};

class DoorList
{
	std::vector<Door> m_doors;
	void ReadCsvTable(CsvTable doorsTable);
public:
	DoorList(CsvTable doorsTable);
	void WriteHTMLReport(const char* folder) const;
	void WriteTigerStopCsvs(const char* folder) const;
	void Print();
	double GetTotalPerimeter() const
	{
		double perimeter = 0.0;
		for (const auto& door : m_doors)
		{
			perimeter += door.GetPerimeter();
		}
		return perimeter;
	}

};


inline StockGroup GetStockGroup(ShakerPart part)
{
	switch (part)
	{
	case ShakerPart::TOP_RAIL:
	case ShakerPart::BOTTOM_RAIL:
	case ShakerPart::MID_RAIL:
		return StockGroup::Rail;

	case ShakerPart::LEFT_STILE:
	case ShakerPart::RIGHT_STILE:
	case ShakerPart::MID_STILE:
		return StockGroup::Stile;

	default:
		return StockGroup::Rail; // safe fallback
	}
}



inline std::string FormatTrimmed(double value)
{
	// 1. Round to 2 decimal places
	std::string s = std::format("{:.2f}", value);

	// 2. Remove trailing zeros
	if (s.find('.') != std::string::npos)
	{
		s.erase(s.find_last_not_of('0') + 1, std::string::npos);
		// 3. Remove trailing decimal point if it's the last character
		if (s.back() == '.')
		{
			s.pop_back();
		}
	}
	return s;
}

inline std::string MakeTigerStopFilename(StockGroup group, double width, std::string jobname)
{
	std::string g = (group == StockGroup::Rail) ? "Rails" : "Stiles";
	std::string s_width = FormatTrimmed(width);
	std::string formatted_str = std::format("{} {} {}inch", jobname, g, s_width);
	return formatted_str;
}