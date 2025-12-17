#pragma once
#include <vector>

constexpr size_t MAXTEXTSIZE = 32;
constexpr double ALLOWANCE = 0.015625;
struct CsvRow;
struct TigerStopCut;
struct CsvTable;

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

enum ShakerPart
{
	TOP_RAIL,
	BOTTOM_RAIL,
	LEFT_STILE,
	RIGHT_STILE,
	MID_RAIL,
	MID_STILE,
	SHAKERPARTCOUNT
};

struct ShakerParts
{
	double width[SHAKERPARTCOUNT] = {};
	double rabbet = 0.0;
	double stick_tolerance = 0.0;
	double cope_tolerance = 0.0;
	unsigned int mid_rail_count = 0;
	unsigned int mid_stile_count = 0;
	double GetPartWidth(ShakerPart part) const
	{ 
		switch(part)
		{
		case TOP_RAIL:
		case BOTTOM_RAIL:
		case LEFT_STILE:
		case RIGHT_STILE:
			return width[part] - stick_tolerance;
		case MID_RAIL:
		case MID_STILE:
			return width[part] - (stick_tolerance * 2.0);
		default:
			return 0.0;
		}
	}
	double GetCutLength(ShakerPart part, double doorWidth, double doorHeight) const
	{
		switch (part)
		{
		case TOP_RAIL:
		case BOTTOM_RAIL:
		case MID_RAIL:
			return doorWidth - GetPartWidth(LEFT_STILE) - GetPartWidth(RIGHT_STILE) + (rabbet * 2) + cope_tolerance * 2;
		case LEFT_STILE:
		case RIGHT_STILE:
			return doorHeight;
		case MID_STILE:
			return doorHeight - GetPartWidth(TOP_RAIL) - GetPartWidth(BOTTOM_RAIL) - (GetPartWidth(MID_RAIL) * mid_rail_count) + (rabbet * 2) + cope_tolerance * 2;
		default:
			return 0.0;
		}
	}
};

struct Panel
{
	Orientation orientation = Orientation::VERTICAL;
	bool hasPanel = false;
private:
	double getWidth(ShakerParts parts, double doorWidth) const
	{
		return doorWidth - parts.GetPartWidth(LEFT_STILE) - parts.GetPartWidth(RIGHT_STILE) - parts.GetPartWidth(MID_STILE) * parts.mid_stile_count - ALLOWANCE;
	}
	double getHeight(ShakerParts parts, double doorHeight) const
	{
		return doorHeight - parts.GetPartWidth(TOP_RAIL) - parts.GetPartWidth(BOTTOM_RAIL) - parts.GetPartWidth(MID_RAIL) * parts.mid_rail_count - ALLOWANCE;
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
	void Create(const CsvRow& row);
	void Print() const;
	void AppendTigerStopCuts(std::vector<TigerStopCut>& cuts) const;
};

class DoorList
{
	std::vector<Door> m_doors;
public:
	void ReadCsvTable(CsvTable doorsTable);
	//void WriteTigerStopCsvs(const char* folder) const;

};