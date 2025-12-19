#pragma once
#include "CsvUtils.h"

//constants
constexpr size_t MAXTEXTSIZE = 32;
constexpr double ALLOWANCE = 0.015625;

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
			case ShakerPart::MID_RAIL:
				return doorWidth - GetPartWidth(ShakerPart::LEFT_STILE) - GetPartWidth(ShakerPart::RIGHT_STILE) + (rabbet * 2) + cope_tolerance * 2;
			case ShakerPart::LEFT_STILE:
			case ShakerPart::RIGHT_STILE:
				return doorHeight;
			case ShakerPart::MID_STILE:
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
			case ShakerPart::MID_RAIL:
				return doorWidth;
			case ShakerPart::LEFT_STILE:
			case ShakerPart::RIGHT_STILE:
			case ShakerPart::MID_STILE:
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
	double getWidth(ShakerParts parts, double doorWidth) const
	{
		return doorWidth - parts.GetPartWidth(ShakerPart::LEFT_STILE) - parts.GetPartWidth(ShakerPart::RIGHT_STILE) - parts.GetPartWidth(ShakerPart::MID_STILE) * parts.mid_stile_count - ALLOWANCE;
	}
	double getHeight(ShakerParts parts, double doorHeight) const
	{
		return doorHeight - parts.GetPartWidth(ShakerPart::TOP_RAIL) - parts.GetPartWidth(ShakerPart::BOTTOM_RAIL) - parts.GetPartWidth(ShakerPart::MID_RAIL) * parts.mid_rail_count - ALLOWANCE;
	}
public:
	double GetPanelWidth(ShakerParts parts, double doorWidth, double doorHeight) const
	{
		if (hasPanel)
		{
			if (orientation == Orientation::VERTICAL)
				return getWidth(parts, doorWidth);
			else
				return getHeight(parts, doorHeight);

		}
		else 
			return 0.0;
	}
	double GetPanelHeight(ShakerParts parts, double doorWidth, double doorHeight) const
	{
		if (hasPanel)
		{
			if (orientation == Orientation::VERTICAL)
				return getHeight(parts, doorHeight);
			else
				return getWidth(parts, doorWidth);
		}
		else
			return 0.0;
	}
	unsigned int GetPanelCount(ShakerParts parts) const
	{
		if (hasPanel)
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
	ShakerParts shakerparts;
	Panel panel;
	double GetOversizedWidth() const { return finishedWidth + oversizeWidth; }
	double GetOversizedHeight() const { return finishedHeight + oversizeHeight; }
	double GetFinishedWidth() const { return finishedWidth; }
	double GetFinishedHeight() const { return finishedHeight; }
};

class Door
{
	char name[MAXTEXTSIZE];
	char label[MAXTEXTSIZE];
	char notes[MAXTEXTSIZE];
	Dimensions dimensions;
	unsigned int quantity = 0;
	Construction construction;
	FaceType type;
	void PrintType() const;
	void PrintPart(ShakerPart part) const;
	void PrintConstruction() const;


public:
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
	void WriteTigerStopCsvs(const char* folder) const;
	void Print();

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