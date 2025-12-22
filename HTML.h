#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <regex>
#include <algorithm>
#undef min


class Fraction
{
    static bool isEven(int number) { return (number % 2 == 0); }
    double decimalvalue;
    int whole;
    int numerator;
    int denominator;
    int direction;   // -1 = down, 0 = exact, +1 = up
public:
    Fraction(double val, int denom)
        : decimalvalue(val), whole(0), numerator(0), denominator(denom), direction(0)
    {
        double scaled = val * denom;
        int rounded = static_cast<int>(std::round(scaled));

        whole = rounded / denom;
        numerator = rounded % denom;

        if (numerator != 0)
            while (isEven(numerator) && numerator != 0 && denominator != 0)
            {
                numerator /= 2;
                denominator /= 2;
            }
        double roundeddecimalvalue = static_cast<double>(whole) + (static_cast<double>(numerator) / static_cast<double>(denominator));
        if (roundeddecimalvalue > val)
            direction++;
        else if (roundeddecimalvalue < val)
            direction--;
    }

    std::string FormatDecimal(double value)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(4) << value;
        std::string s = oss.str();

        // Remove trailing zeros
        s = std::regex_replace(s, std::regex("0+$"), "");

        // If ends with '.', remove it
        if (!s.empty() && s.back() == '.')
            s.pop_back();

        return s;
    }
    std::string GetDecimalString(bool usedash = false)
    {
        if (decimalvalue == 0.0)
        {
            if (usedash)
                return "---";
            else
                return "0\"";
        }
        return FormatDecimal(decimalvalue) + "\"";
    }
    std::string GetFractionString()
    {
        if (numerator == 0)
            return std::to_string(whole) + "\"";
        else if (whole == 0)
			return std::to_string(numerator) + "/" + std::to_string(denominator) + "\"";

        return std::to_string(whole) + " " + std::to_string(numerator) + "/" + std::to_string(denominator) + "\"";
    }
    std::string GetRoundingString()
    {
        if (direction < 0) return "rounded down";
        else if (direction > 0) return "rounded up";
        else return "";
    }
    std::string GetString(bool usedash = false)
    {
        if (decimalvalue == 0.0)
        {
            if (usedash)
                return "---";
			else
			    return "0\"";
        }
        else if (direction == 0 && numerator != 0)
            return GetDecimalString() + " (" + GetFractionString() + ")";
        else if (direction == 0 && numerator == 0)
            return GetDecimalString();
        return GetDecimalString() + " (" + GetFractionString() + ", " + GetRoundingString() + ")";
	}
};

namespace Html
{
    // ============================================================
    // Utility helpers
    // ============================================================

    namespace Util
    {
        inline std::string Escape(const std::string& text)
        {
            std::string out;
            out.reserve(text.size());

            for (char c : text)
            {
                switch (c)
                {
                case '&':  out += "&amp;";  break;
                case '<':  out += "&lt;";   break;
                case '>':  out += "&gt;";   break;
                case '"':  out += "&quot;"; break;
                case '\'': out += "&#39;";  break;
                default:   out += c;        break;
                }
            }
            return out;
        }
    }

    // ============================================================
    // HtmlTable
    // ============================================================
    class HtmlTable
    {
    public:
        struct Cell
        {
            std::string content;
            int colspan = 1;
            int rowspan = 1;
            bool rightAlign = false;

            Cell(const std::string& text,
                int cs = 1,
                int rs = 1,
                bool alignRight = false)
                : content(text), colspan(cs), rowspan(rs), rightAlign(alignRight)
            {}
        };

        struct Column
        {
            std::string header;
            std::string width;      // e.g. "40%", "120px"
            bool rightAlign = false;
        };

        HtmlTable& AddColumn(const Column& col)
        {
            m_columns.push_back(col);
            return *this;
        }

        HtmlTable& AddRow(const std::vector<std::string>& cells)
        {
            std::vector<Cell> row;
            for (const auto& c : cells)
                row.emplace_back(Util::Escape(c));
            m_rows.push_back(row);
            return *this;
        }

        HtmlTable& AddKeyValue(const std::string& key,
            const std::string& value)
        {
            m_rows.push_back({
                Cell(Util::Escape(key)),
                Cell(Util::Escape(value))
                });
            return *this;
        }


        HtmlTable& BeginRow()
        {
            m_rows.emplace_back();
            return *this;
        }

        HtmlTable& AddCell(const Cell& cell)
        {
            if (m_rows.empty())
                m_rows.emplace_back();

            m_rows.back().push_back(cell);
            return *this;
        }


        std::string ToHtml() const
        {
            std::ostringstream html;
            html << "<table>\n";

            if (!m_columns.empty())
            {
                html << "<thead><tr>";
                for (const auto& col : m_columns)
                {
                    html << "<th";
                    if (!col.width.empty())
                        html << " style='width:" << col.width << ";'";
                    html << ">";
                    html << Util::Escape(col.header);
                    html << "</th>";
                }
                html << "</tr></thead>\n";
            }

            html << "<tbody>\n";
            for (const auto& row : m_rows)
            {
                html << "<tr>";
                size_t colIndex = 0;

                for (const auto& cell : row)
                {
                    html << "<td";

                    if (cell.colspan > 1)
                        html << " colspan='" << cell.colspan << "'";
                    if (cell.rowspan > 1)
                        html << " rowspan='" << cell.rowspan << "'";

                    bool alignRight = cell.rightAlign ||
                        (colIndex < m_columns.size() && m_columns[colIndex].rightAlign);

                    if (alignRight)
                        html << " style='text-align:right;'";

                    html << ">";
                    html << cell.content;
                    html << "</td>";

                    colIndex += cell.colspan;
                }
                html << "</tr>\n";
            }
            html << "</tbody></table>\n";


            return html.str();
        }

    private:
        std::vector<Column> m_columns;
        std::vector<std::vector<Cell>> m_rows;

    };

    // ============================================================
    // HtmlDocument
    // ============================================================
    class HtmlDocument
    {
    public:
        explicit HtmlDocument(const std::string& title)
            : m_title(title)
        {
            // Default styles: print-safe, shop-friendly
            m_styles = R"(
                body {
                    font-family: Arial, sans-serif;
                    font-size: 10px;
                }
                h1, h2, h3 {
                    margin: 1px 0;
                }
                table {
                    border-collapse: collapse;
                    width: 100%;
                    margin-bottom: 0px;
                }
                th, td {
                    border: 1px solid #000;
                    padding: 0px 0px;
                    vertical-align: top;
                }
                //th {
                //    background: #ffffff;
                //}
                @media print {
                    body {
                        margin: 0.0in;
                    }
                }
                @page {
                    background-color: #f3f3f3;
                    size: Letter;
                    margin: 0.0in;
                }
                
                /* SCREEN PREVIEW */
                @media screen {
                    body {
                        background: #ccc;
                    }
                
                    .page {
                        width: 8.5in;
                        margin: 0 auto;
                        background: white;
                        box-shadow: 0 0 10px rgba(0,0,0,0.4);
                    }
                
                    .page-inner {
                        margin: 0 auto;
                    }
                }
                
                /* PRINT: REMOVE PREVIEW PADDING */
                @media print {
                    body {
                        background: white;
                        margin: 0;
                    }
                
                    .page {
                        width: auto;
                        margin: 0;
                        box-shadow: none;
                    }
                
                    .page-inner {
                        padding: 0; 
                    }
                }

            )";
        }

        // ---------------- Document structure ----------------

        void AddStyle(const std::string& css)
        {
            m_styles += "\n" + css;
        }

        void AddHeading(const std::string& text, int level = 1)
        {
            if (level < 1) level = 1;
            if (level > 6) level = 6;

            m_body += "<h" + std::to_string(level) + ">"
                + Util::Escape(text)
                + "</h" + std::to_string(level) + ">\n";
        }

        void AddParagraph(const std::string& text)
        {
            m_body += "<p>" + Util::Escape(text) + "</p>\n";
        }

        void AddRawHtml(const std::string& html)
        {
            m_body += html + "\n";
        }

        void AddTable(const HtmlTable& table)
        {
            m_body += table.ToHtml();
        }

        void AddPageBreak()
        {
            m_body += "<div style='page-break-after: always;'></div>\n";
        }

        // ---------------- Grid / Block helpers ----------------

        void BeginGrid(const std::string& className)
        {
            m_body += "<div class='" + className + "'>\n";
        }

        void EndGrid()
        {
            m_body += "</div>\n";
        }

        void BeginBlock(const std::string& className)
        {
            m_body += "<div class='" + className + "'>\n";
        }

        void EndBlock()
        {
            m_body += "</div>\n";
        }

        // ---------------- Output ----------------

        std::string ToString() const
        {
            std::ostringstream html;

            html << "<!DOCTYPE html>\n";
            html << "<html>\n<head>\n";
            html << "<meta charset='utf-8'>\n";
            html << "<title>" << Util::Escape(m_title) << "</title>\n";
            html << "<style>\n" << m_styles << "\n</style>\n";
            html << "<body>\n";
            html << "<div class='page'><div class='page-inner'>\n";
            html << m_body;
            html << "</div></div>\n";
            html << "</body>\n";


            return html.str();
        }

        bool WriteToFile(const std::string& path) const
        {
            std::ofstream file(path, std::ios::out | std::ios::trunc);
            if (!file.is_open())
                return false;

            file << ToString();
            return true;
        }

    private:
        std::string m_title;
        std::string m_styles;
        std::string m_body;
    };
}

namespace Html
{
    namespace Svg
    {
        enum class DoorStyle
        {
            Slab,
            Shaker,
            ShakerMitered
        };

        class DoorDiagram;
    }
}

namespace Html::Svg
{
    class DoorDiagram
    {
    public:
        DoorDiagram()
            : m_width(120), m_height(180),
            m_vbW(100), m_vbH(160),
            m_style(DoorStyle::Slab),
            m_TopRail(12), m_BottomRail(12),
            m_LeftStile(12), m_RightStile(12),
            m_MidWidth(12), 
            m_bHasMidRail(false),
            m_stroke(1.5)
        {}

        DoorDiagram& SetSize(int pxW, int pxH)
        {
            m_width = pxW;
            m_height = pxH;
            return *this;
        }

        DoorDiagram& SetViewBox(double x, double y, double w, double h)
        {
            m_vbX = x; m_vbY = y;
            m_vbW = w; m_vbH = h;
            return *this;
        }

        DoorDiagram& SetDoorStyle(DoorStyle style)
        {
            m_style = style;
            return *this;
        }

        DoorDiagram& SetTopRailWidth(double w)
        {
            m_TopRail = w;
            return *this;
        }

        DoorDiagram& SetBottomRailWidth(double w)
        {
            m_BottomRail = w;
            return *this;
        }

        DoorDiagram& SetLeftStileWidth(double w)
        {
            m_LeftStile = w;
            return *this;
        }

        DoorDiagram& SetRightStileWidth(double w)
        {
            m_RightStile = w;
            return *this;
        }

        DoorDiagram& SetMidWidth(double w)
        {
            m_MidWidth = w;
            return *this;
        }

        DoorDiagram& SetMidRail(bool enable)
        {
            m_bHasMidRail = enable;
            return *this;
        }

        DoorDiagram& SetStrokeWidth(double w)
        {
            m_stroke = w;
            return *this;
        }

        DoorDiagram& SetLabel(const std::string& text)
        {
            m_label = text;
            return *this;
        }

        std::string ToHtml() const
        {
            std::ostringstream svg;

            svg << "<svg class='door-diagram' "
                << "width='" << m_width << "' "
                << "height='" << m_height << "' "
                << "viewBox='" << m_vbX << " " << m_vbY << " "
                << m_vbW << " " << m_vbH << "' "
                << "preserveAspectRatio = 'xMidYMid meet'"
                << "xmlns='http://www.w3.org/2000/svg'>\n";

            DrawOuter(svg);

            if (m_style != DoorStyle::Slab)
                DrawFrame(svg);

            DrawLabel(svg);

            svg << "</svg>";

            return svg.str();
        }

    private:
        int m_width, m_height;
        double m_vbX = 0, m_vbY = 0, m_vbW, m_vbH;
        DoorStyle m_style;
        double m_TopRail;
        double m_BottomRail;
        double m_LeftStile;
        double m_RightStile;
        double m_MidWidth;
        bool m_bHasMidRail;
        double m_stroke;
        std::string m_label;
        double m_Scale_Factor = 0.9;

        void DrawOuter(std::ostringstream& svg) const
        {
            double w = m_vbW;
            double h = m_vbH;
            svg << Rect(0,0,w,h);
        }

        void DrawFrame(std::ostringstream& svg) const
        {
            double w = m_vbW;
            double h = m_vbH;
            if (m_style == DoorStyle::Shaker)
            {
                // Stiles
                svg << Rect(0, 0, m_LeftStile, h);
                svg << Rect(w - m_RightStile , 0, m_RightStile, h);

                // Rails
                svg << Rect(m_LeftStile, 0, w - (m_LeftStile + m_RightStile) , m_TopRail);
                svg << Rect(m_LeftStile, h-m_BottomRail, w - (m_LeftStile + m_RightStile), m_BottomRail);

                //if (m_midRail)
                //    svg << Rect(m_stile + 2, h / 2 - m_rail / 2,
                //        w - 2 * m_stile - 4, m_rail);

            }


            if (m_style == DoorStyle::ShakerMitered)
                DrawMiterSmallShaker(svg);
        }
        double scaleX(double x) const
        {
            double cx = m_vbX + m_vbW / 2.0;
            return cx + (x - cx) * m_Scale_Factor;
        }

        double scaleY(double y) const
        {
            double cy = m_vbY + m_vbH / 2.0;
            return cy + (y - cy) * m_Scale_Factor;
        }

        void DrawMiterSmallShaker(std::ostringstream& svg) const
        {
            double w = m_vbW;
            double h = m_vbH;
            svg << Line(0, 0, m_LeftStile, m_TopRail);
            svg << Line(w - m_RightStile, m_TopRail, w, 0);
            svg << Line(0, h, m_LeftStile, h - m_BottomRail);
            svg << Line(w - m_RightStile, h - m_BottomRail, w, h);
            svg << Rect(m_LeftStile, m_TopRail, w - (m_LeftStile + m_RightStile), h - (m_TopRail + m_BottomRail));
        }

        std::string Rect(double x, double y, double w, double h) const
        {
            x = scaleX(x);
            y = scaleY(y);
            w = w * m_Scale_Factor;
            h = h * m_Scale_Factor;
            std::ostringstream r;
            r << "<rect x='" << x << "' y='" << y
                << "' width='" << w << "' height='" << h
                << "' fill='none' stroke='black' "
                << "stroke-width='" << m_stroke << "'/>\n";
            return r.str();
        }
        std::string Line(double x1, double y1, double x2, double y2) const
        {
            x1 = scaleX(x1);
            x2 = scaleX(x2);
            y1 = scaleY(y1);
            y2 = scaleY(y2);
            std::ostringstream r;
            r << "<line x1='" << x1 << "' y1='" << y1 << " ' x2 ='" << x2 << "' y2 ='" << y2 
                << "' fill='none' stroke='black' "
                << "stroke-width='" << m_stroke << "'/>\n";
            return r.str();
        }

        void DrawLabel(std::ostringstream& svg) const
        {
            double cx = m_vbX + m_vbW / 2.0;
            double cy = m_vbY + m_vbH / 2.0;

            // How much the SVG is being scaled on screen
            double scaleX = static_cast<double>(m_width) / m_vbW;
            double scaleY = static_cast<double>(m_height) / m_vbH;
            double scale = std::min(scaleX, scaleY);

            double desiredPx = 10.0;
            double fontUnits = desiredPx / scale;   // convert px → SVG units

            svg
                << "<text x='" << cx
                << "' y='" << cy
                << "' text-anchor='middle'"
                << " dominant-baseline='middle'"
                << " font-size='" << fontUnits << "'"
                << " fill='black'>"
                << m_label
                << "</text>\n";
        }
    };
}
