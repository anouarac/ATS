//
// Created by Anouar Achghaf on 05/03/2023.
//
#include "Plotter.h"

BinanceAPI::BinanceAPI(ats::BinanceExchangeManager &ems_) : ems(ems_) {}

TickerData BinanceAPI::get_ticker(std::string ticker, ImPlotTime start_date, ImPlotTime end_date, Interval interval) {
    std::transform(ticker.begin(), ticker.end(), ticker.begin(), ::toupper);
    Json::Value result;


    ems.getKlines(result, ticker, getStrInterval(interval));

    try {
        TickerData data(ticker);
        for (Json::Value::ArrayIndex i = 0; i < result.size(); i++) {
            if (result[i].size() < 6) return TickerData("ERROR");
            double t = jsonToDouble(result[i][0])/1000;
            double o = jsonToDouble(result[i][1]);
            double h = jsonToDouble(result[i][2]);
            double l = jsonToDouble(result[i][3]);
            double c = jsonToDouble(result[i][4]);
            double v = jsonToDouble(result[i][5]);
            data.push_back(t, o, h, l, c, v);
        }
        return data;
    }
    catch (...) {
        return TickerData("ERROR");
    }
}

TickerData BinanceAPI::update_ticker(std::string ticker, Interval interval) {
    std::transform(ticker.begin(), ticker.end(), ticker.begin(), ::toupper);
    Json::Value result;


    ems.getKlines(result, ticker, getStrInterval(interval), 0, 0, 10);

    try {
        TickerData data(ticker);
        for (Json::Value::ArrayIndex i = 0; i < result.size(); i++) {
            double t = jsonToDouble(result[i][0])/1000;
            double o = jsonToDouble(result[i][1]);
            double h = jsonToDouble(result[i][2]);
            double l = jsonToDouble(result[i][3]);
            double c = jsonToDouble(result[i][4]);
            double v = jsonToDouble(result[i][5]);
            data.push_back(t, o, h, l, c, v);
        }
        return data;
    }
    catch (...) {
        return TickerData("ERROR");
    }
}

std::vector<std::pair<std::string,double>> BinanceAPI::get_balances() {
    Json::Value result;
    ems.getUserInfo(result);
    if (!result.isMember("balances")) return {};
    std::vector<std::pair<std::string,double>> balances;
    for (Json::Value::ArrayIndex i = 0; i < result["balances"].size(); i++) {
        auto res = result["balances"][i];
        balances.push_back(std::make_pair(res["asset"].asString(), jsonToDouble(res["free"])));
    }
    return balances;
}

ats::OrderBook BinanceAPI::get_order_book(std::string ticker) {
    ats::OrderBook ob = ems.getOrderBook(ticker);
    reverse(ob.ask.begin(), ob.ask.end());
    reverse(ob.askVol.begin(), ob.askVol.end());
    return ob;
}

std::string BinanceAPI::getStrInterval(Interval interval) {
    static const std::string interval_str[]{"1s",
                                            "1m",
                                            "3m",
                                            "5m",
                                            "15m",
                                            "30m",
                                            "1h",
                                            "2h",
                                            "4h",
                                            "6h",
                                            "8h",
                                            "12h",
                                            "1d",
                                            "3d",
                                            "1w",
                                            "1M"};
    return interval_str[interval];
}

double BinanceAPI::jsonToDouble(Json::Value res) {
    return atof(res.asString().c_str());
}

std::vector<ats::Order> BinanceAPI::get_open_orders(std::string ticker) {
    return ems.getOpenOrders(ticker);
}

std::time_t timestamp_from_string(std::string date, const char *format = "%Y-%m-%d") {
    struct std::tm time = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::istringstream ss(date);
    ss >> std::get_time(&time, format);
    if (ss.fail()) {
        std::cerr << "ERROR: Cannot parse date string (" << date << "); required format %Y-%m-%d" << std::endl;
        exit(1);
    }

    time.tm_hour = 0;
    time.tm_min = 0;
    time.tm_sec = 0;
#ifdef _WIN32
    return _mkgmtime(&time);
#else
    return timegm(&time);
#endif
}

void TickerTooltip(const TickerData &data, bool span_subplots, Interval interval) {
    ImDrawList *draw_list = ImPlot::GetPlotDrawList();
    std::vector<double> coef(Interval_1M+1, 1);
    coef[0] = 1. / 24 / 60 / 60;
    coef[1] = coef[0] * 60;
    coef[2] = coef[1] * 3;
    coef[3] = coef[1] * 5;
    coef[4] = coef[1] * 15;
    coef[5] = coef[1] * 30;
    coef[6] = coef[5] * 2;
    coef[7] = coef[6] * 2;
    coef[8] = coef[7] * 2;
    coef[9] = coef[7] * 3;
    coef[10] = coef[8] * 2;
    coef[11] = coef[9] * 2;
    coef[12] = coef[11] * 2;
    coef[13] = coef[12] * 3;
    coef[14] = coef[12] * 7;
    coef[15] = coef[12] * 30;
    const double half_width = 24 * 60 * 60 * 0.25 * 1.5 * coef[interval];
    const bool hovered = span_subplots ? ImPlot::IsSubplotsHovered() : ImPlot::IsPlotHovered();
    if (hovered) {
        ImPlotPoint mouse = ImPlot::GetPlotMousePos();
        mouse.x = ImPlot::RoundTime(ImPlotTime::FromDouble(mouse.x), ImPlotTimeUnit_S).ToDouble();
        float tool_l = ImPlot::PlotToPixels(mouse.x - half_width, mouse.y).x;
        float tool_r = ImPlot::PlotToPixels(mouse.x + half_width, mouse.y).x;
        float tool_t = ImPlot::GetPlotPos().y;
        float tool_b = tool_t + ImPlot::GetPlotSize().y;
        ImPlot::PushPlotClipRect();
        draw_list->AddRectFilled(ImVec2(tool_l, tool_t), ImVec2(tool_r, tool_b), IM_COL32(128, 128, 128, 64));
        ImPlot::PopPlotClipRect();
        // find mouse location index
        int idx = BinarySearch(data.time.data(), 0, data.size() - 1, mouse.x);
        // render tool tip (won't be affected by plot clip rect)
        if (ImPlot::IsPlotHovered() && idx != -1) {
            ImGui::BeginTooltip();
            char buff[32];
            ImPlot::FormatDate(ImPlotTime::FromDouble(data.time[idx]), buff, 32, ImPlotDateFmt_DayMoYr,
                               ImPlot::GetStyle().UseISO8601);
            ImGui::Text("Date:");
            ImGui::SameLine(60);
            ImGui::Text("%s", buff);
            ImGui::Text("Open:");
            ImGui::SameLine(60);
            ImGui::Text("$%.2f", data.open[idx]);
            ImGui::Text("Close:");
            ImGui::SameLine(60);
            ImGui::Text("$%.2f", data.close[idx]);
            ImGui::Text("High:");
            ImGui::SameLine(60);
            ImGui::Text("$%.2f", data.high[idx]);
            ImGui::Text("Low:");
            ImGui::SameLine(60);
            ImGui::Text("$%.2f", data.low[idx]);
            ImGui::Text("Volume:");
            ImGui::SameLine(60);
            ImGui::Text(fmt::format(std::locale("en_US.UTF-8"), "{:L}", (int) (data.volume[idx])).c_str());
            ImGui::EndTooltip();
        }
    }
}


void PlotOHLC(const char *label_id, const TickerData &data, ImVec4 bullCol,
              ImVec4 bearCol, Interval interval) {

    // get ImGui window DrawList
    ImDrawList *draw_list = ImPlot::GetPlotDrawList();

    std::vector<double> coef(Interval_1M+1, 1);
    coef[0] = 1. / 24 / 60 / 60;
    coef[1] = coef[0] * 60;
    coef[2] = coef[1] * 3;
    coef[3] = coef[1] * 5;
    coef[4] = coef[1] * 15;
    coef[5] = coef[1] * 30;
    coef[6] = coef[5] * 2;
    coef[7] = coef[6] * 2;
    coef[8] = coef[7] * 2;
    coef[9] = coef[7] * 3;
    coef[10] = coef[8] * 2;
    coef[11] = coef[9] * 2;
    coef[12] = coef[11] * 2;
    coef[13] = coef[12] * 3;
    coef[14] = coef[12] * 7;
    coef[15] = coef[12] * 30;
    // calc real value width
    const double half_width = 24 * 60 * 60 * 0.48 * coef[interval];
    // begin plot item
    if (ImPlot::BeginItem(label_id)) {
        // override legend icon color
        ImPlot::GetCurrentItem()->Color = ImGui::GetColorU32(bullCol);
        // fit data if requested
        if (ImPlot::FitThisFrame()) {
            for (int i = 0; i < data.size(); ++i) {
                ImPlot::FitPoint(ImPlotPoint(data.time[i], data.low[i]));
                ImPlot::FitPoint(ImPlotPoint(data.time[i], data.high[i]));
            }
        }
        // render data
        for (int i = 0; i < data.size(); ++i) {
            ImU32 color = ImGui::GetColorU32(data.open[i] > data.close[i] ? bearCol : bullCol);
            ImVec2 open_pos = ImPlot::PlotToPixels(data.time[i] - half_width, data.open[i]);
            ImVec2 close_pos = ImPlot::PlotToPixels(data.time[i] + half_width, data.close[i]);
            ImVec2 low_pos = ImPlot::PlotToPixels(data.time[i], data.low[i]);
            ImVec2 high_pos = ImPlot::PlotToPixels(data.time[i], data.high[i]);
            float line_width = ImMax(1.0f, ImAbs(open_pos.x - close_pos.x) / 10.0f);

            // draw the outline lines
            float outline_width = line_width/10.0f; // adjust the outline width as desired
            ImU32 outline_color = IM_COL32_BLACK; // set the outline color
            draw_list->AddLine(low_pos + ImVec2(1.0f, 0.0f)*line_width/2, high_pos + ImVec2(1.0f, 0.0f)*line_width/2, outline_color);//, outline_width);
            draw_list->AddLine(low_pos + ImVec2(-1.0f, 0.0f)*line_width/2, high_pos + ImVec2(-1.0f, 0.0f)*line_width/2, outline_color);//, outline_width);
            draw_list->AddLine(high_pos + ImVec2(-1.0f, 0.0f)*line_width/2, high_pos + ImVec2(1.0f, 0.0f)*line_width/2, outline_color);//, outline_width);
            draw_list->AddLine(low_pos + ImVec2(-1.0f, 0.0f)*line_width/2, low_pos + ImVec2(1.0f, 0.0f)*line_width/2, outline_color);//, outline_width);
            draw_list->AddRectFilled(open_pos, close_pos, color);
            // draw outline
            ImU32 line_color = IM_COL32_BLACK;
            draw_list->AddRect(open_pos, close_pos, line_color);
            // draw the main line
            draw_list->AddLine(low_pos, high_pos, color, line_width);
        }
        // end plot item
        ImPlot::EndItem();
    }
}

int VolumeFormatter(double value, char *buff, int size, void *) {
    static double v[] = {1000000000000, 1000000000, 1000000, 1000, 1};
    static const char *p[] = {"T", "B", "M", "k", ""};
    if (value == 0) {
        return snprintf(buff, size, "0");
    }
    for (int i = 0; i < 5; ++i) {
        if (fabs(value) >= v[i]) {
            return snprintf(buff, size, "%g%s", value / v[i], p[i]);
        }
    }
    return snprintf(buff, size, "%g%s", value / v[4], p[4]);
}
