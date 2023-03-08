//
// Created by Anouar Achghaf on 05/03/2023.
//
#ifndef ATS_PLOTTER_H
#define ATS_PLOTTER_H

#include "App.h"
#include "ats.h"
#include <string.h>
#include <string.h>
#include "curl/curl.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <stdio.h>
#include "implot_internal.h"
#include "fmt/format.h"

enum Interval {
    Interval_1s,
    Interval_1m,
    Interval_3m,
    Interval_5m,
    Interval_15m,
    Interval_30m,
    Interval_1h,
    Interval_2h,
    Interval_4h,
    Interval_6h,
    Interval_8h,
    Interval_12h,
    Interval_1d,
    Interval_3d,
    Interval_1w,
    Interval_1M,
};

struct TickerData {

    TickerData() = default;

    TickerData(std::string ticker) : ticker(ticker) {}

    void reserve(int n) {
        time.reserve(n);
        open.reserve(n);
        high.reserve(n);
        low.reserve(n);
        close.reserve(n);
        volume.reserve(n);
    }

    void push_back(double t, double o, double h, double l, double c, double v) {
        time.push_back(t);
        open.push_back(o);
        high.push_back(h);
        low.push_back(l);
        close.push_back(c);
        volume.push_back(v);

        int s = size();
        int i = std::max(0, s - 20);
        double mean = ImMean(&close[i], std::min(s, 20));
        double stdv = s > 1 ? ImStdDev(&close[i], std::min(s, 20)) : 0;

        bollinger_top.push_back(mean - 2 * stdv);
        bollinger_mid.push_back(mean);
        bollinger_bot.push_back(mean + 2 * stdv);

    }

    void push_back(TickerData d) {
        for (int i = 0; i < d.size(); i++)
            push_back(d.time[i], d.open[i], d.high[i], d.low[i], d.close[i], d.volume[i]);
    }

    void pop_back() {
        time.pop_back();
        open.pop_back();
        high.pop_back();
        low.pop_back();
        close.pop_back();
        volume.pop_back();

        bollinger_top.pop_back();
        bollinger_mid.pop_back();
        bollinger_bot.pop_back();

    }

    void pop_front() {
        time.erase(time.begin());
        open.erase(open.begin());
        high.erase(high.begin());
        low.erase(low.begin());
        close.erase(close.begin());
        volume.erase(volume.begin());

        bollinger_top.erase(bollinger_top.begin());
        bollinger_mid.erase(bollinger_mid.begin());
        bollinger_bot.erase(bollinger_bot.begin());
    }

    int size() const {
        return (int) time.size();
    }

    Interval interval;
    std::string ticker;
    std::vector<double> time;
    std::vector<double> open;
    std::vector<double> high;
    std::vector<double> low;
    std::vector<double> close;
    std::vector<double> volume;

    std::vector<double> bollinger_top;
    std::vector<double> bollinger_mid;
    std::vector<double> bollinger_bot;

};

class StockAPI {
public:
    virtual ~StockAPI() {}
};

class BinanceAPI {
    ats::BinanceExchangeManager &ems;

public:
    BinanceAPI(ats::BinanceExchangeManager &ems_);

    TickerData get_ticker(std::string ticker, ImPlotTime start_date, ImPlotTime end_date, Interval interval);

    TickerData update_ticker(std::string ticker, Interval interval);

    std::vector<ats::Order> get_open_orders(std::string ticker);

    std::vector<std::pair<std::string, double>> get_balances();

    std::string getStrInterval(Interval interval);

    double jsonToDouble(Json::Value);
};

template<typename T>
int BinarySearch(const T *arr, int l, int r, T x) {
    if (r >= l) {
        int mid = l + (r - l) / 2;
        if (arr[mid] == x)
            return mid;
        if (arr[mid] > x)
            return BinarySearch(arr, l, mid - 1, x);
        return BinarySearch(arr, mid + 1, r, x);
    }
    return -1;
}

void TickerTooltip(const TickerData &data, bool span_subplots = false, Interval interval = Interval_1h);

void PlotOHLC(const char *label_id, const TickerData &data, ImVec4 bullCol = ImVec4(0, 1, 0, 1),
              ImVec4 bearCol = ImVec4(1, 0, 0, 1), Interval interval = Interval_1h);

int VolumeFormatter(double value, char *buff, int size, void *);

struct ImBinance : App {

    using App::App;

    char t1_str[32];
    char t2_str[32];
    ImPlotTime t1;
    ImPlotTime t2;
    Interval interval = Interval_1h;
    time_t start_time;
    bool m_done_upd;

    void Start() override {
        time(&start_time);
        t2 = ImPlot::FloorTime(ImPlotTime::FromDouble((double) time(0)), ImPlotTimeUnit_Day);
        t1 = ImPlot::AddTime(t2, ImPlotTimeUnit_Mo, -1);
        auto d = m_api.get_ticker("BTCUSDT", t1, t2, interval);
        auto v = m_api.get_open_orders("BTCUSDT");
        auto b = m_api.get_balances();
        if (d.ticker != "ERROR") {
            d.interval = interval;
            m_ticker_data[d.ticker] = d;
            m_open_orders[d.ticker] = v;
            m_balances = b;
        }
        ImPlot::GetStyle().FitPadding.y = 0.2f;
        ImGui::StyleColorsDark();
        m_done_upd = 1;
    }

    void UpdateData() {
        m_done_upd = 0;
        auto m_ticker_data_copy = m_ticker_data;
        auto m_open_orders_copy = m_open_orders;
        auto m_balances_copy = m_balances;
        for (auto &pair: m_ticker_data_copy) {
            auto &data = pair.second;
            auto d = m_api.update_ticker(data.ticker, data.interval);
            auto v = m_api.get_open_orders(data.ticker);
            auto b = m_api.get_balances();
            while (d.size() && data.size() && d.time[0] < data.time.back())
                d.pop_front();
            if (data.size() && d.size() && data.time.back() == d.time[0])
                data.pop_back();
            if (d.ticker != "ERROR") {
                d.interval = interval;
                data.push_back(d);
                m_open_orders_copy[d.ticker] = v;
                m_balances_copy = b;
            }
            ImPlot::GetStyle().FitPadding.y = 0.2f;
        }
        std::lock_guard<std::mutex> lock(m_data_mutex);
        m_open_orders_copy.swap(m_open_orders);
        m_balances_copy.swap(m_balances);
        m_ticker_data_copy.swap(m_ticker_data);
        m_done_upd = 1;
    }

    void Update() override {

        static ImVec4 bull_col(0, 1, 0, 1);
        static ImVec4 bear_col(1, 0, 0, 1);

        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(GetWindowSize(), ImGuiCond_Always);
        ImGui::Begin("##Exchange UI", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize/*|ImGuiWindowFlags_MenuBar*/);


        static char buff[8] = "BTCUSDT";
        ImGui::SetNextItemWidth(200);
        if (ImGui::InputText("##Symbol", buff, 8,
                             ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsUppercase)) {

        }

        time_t end_time;
        time(&end_time);
        if (difftime(end_time, start_time) > 1) {
            if (m_done_upd) {
                if (t.joinable())
                    t.join();
                t = std::thread(&ImBinance::UpdateData, this);
                time(&start_time);
            }
        }

        ImGui::SameLine();

        static int duration = 3;
        ImGui::SetNextItemWidth(200);
        if (ImGui::Combo("##Interval", &duration,
                         "1 Second\0""1 Minute\0""5 Minutes\0""1 Hour\0""1 Day\0""1 Week\0""1 Month\0")) {
            switch (duration) {
                case 0:
                    t1 = ImPlot::AddTime(t2, ImPlotTimeUnit_S, -100);
                    interval = Interval_1s;
                    break;
                case 1:
                    t1 = ImPlot::AddTime(t2, ImPlotTimeUnit_Min, -100);
                    interval = Interval_1m;
                    break;
                case 2:
                    t1 = ImPlot::AddTime(t2, ImPlotTimeUnit_Min, -500);
                    interval = Interval_5m;
                    break;
                case 3:
                    t1 = ImPlot::AddTime(t2, ImPlotTimeUnit_Hr, -100);
                    interval = Interval_1h;
                    break;
                case 4:
                    t1 = ImPlot::AddTime(t2, ImPlotTimeUnit_Day, -100);
                    interval = Interval_1d;
                    break;
                case 5:
                    t1 = ImPlot::AddTime(t2, ImPlotTimeUnit_Day, -700);
                    interval = Interval_1w;
                    break;
                case 6:
                    t1 = ImPlot::AddTime(t2, ImPlotTimeUnit_Mo, -100);
                    interval = Interval_1M;
                    break;
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Fetch")) {
            auto d = m_api.get_ticker(buff, t1, t2, interval);
            d.interval = interval;
            auto v = m_api.get_open_orders(buff);
            std::lock_guard<std::mutex> lock(m_data_mutex);
            if (d.ticker != "ERROR") {
                m_ticker_data[d.ticker] = d;
                m_open_orders[d.ticker] = v;
            } else
                fmt::print("Failed to get data for ticker symbol {}!\n", buff);
        }

        ImGui::SameLine();
        ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);

        ImVec2 windowSize = ImGui::GetWindowSize();

        if (ImGui::BeginTabBar("TickerTabs")) {
            std::lock_guard<std::mutex> lock(m_data_mutex);
            for (auto &pair: m_ticker_data) {
                auto &data = pair.second;
                if (ImGui::BeginTabItem(data.ticker.c_str())) {
                    static float ratios[] = {2, 1};
                    if (ImPlot::BeginSubplots("##Pairs", 2, 1, ImVec2(0.65*windowSize.x, 0.9*windowSize.y), ImPlotSubplotFlags_LinkCols, ratios)) {
                        if (ImPlot::BeginPlot("##OHLCPlot")) {
                            // After changing scale, double click to fit automatically
                            ImPlot::SetupAxes(0, 0, ImPlotAxisFlags_NoTickLabels,
                                              ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit |
                                              ImPlotAxisFlags_Opposite);

                            ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
                            ImPlot::SetupAxisLimits(ImAxis_X1, data.time[0], data.time.back());
                            ImPlot::SetupAxisFormat(ImAxis_Y1, "$%.0f");
                            TickerTooltip(data, true, data.interval);
                            ImPlot::SetNextFillStyle(ImVec4(0.5, 0.5, 1, 1), 0.25f);
                            ImPlot::PlotShaded("BB", data.time.data(), data.bollinger_top.data(),
                                               data.bollinger_bot.data(), data.size());
                            ImPlot::SetNextLineStyle(ImVec4(0.5, 0.5, 1, 1));
                            ImPlot::PlotLine("BB", data.time.data(), data.bollinger_mid.data(), data.size());
                            PlotOHLC("OHLC", data, bull_col, bear_col, data.interval);
                            ImPlot::SetNextLineStyle(ImVec4(1, 1, 1, 1));
                            ImPlot::PlotLine("Close", data.time.data(), data.close.data(), data.size());
                            ImPlotRect bnds = ImPlot::GetPlotLimits();
                            int close_idx = BinarySearch(data.time.data(), 0, data.size() - 1,
                                                         ImPlot::RoundTime(ImPlotTime::FromDouble(bnds.X.Max),
                                                                           ImPlotTimeUnit_Day).ToDouble());
                            if (close_idx == -1)
                                close_idx = data.time.size() - 1;
                            double close_val = data.close[close_idx];
                            ImPlot::TagY(close_val, data.open[close_idx] < data.close[close_idx] ? bull_col : bear_col);
                            ImPlot::EndPlot();
                        }
                        if (ImPlot::BeginPlot("##VolumePlot")) {
                            ImPlot::SetupAxes(0, 0, 0, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit |
                                                       ImPlotAxisFlags_Opposite);
                            ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
                            ImPlot::SetupAxisLimits(ImAxis_X1, data.time[0], data.time.back());
                            ImPlot::SetupAxisFormat(ImAxis_Y1, VolumeFormatter);
                            TickerTooltip(data, true, data.interval);
                            ImPlot::SetNextFillStyle(ImVec4(1.f, 0.75f, 0.25f, 1));
                            std::vector<double> coef(Interval_1M + 1, 1);
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
                            ImPlot::PlotBars("Volume", data.time.data(), data.volume.data(), data.size(),
                                             60 * 60 * 24 * 0.5 * coef[data.interval]);
                            ImPlot::EndPlot();
                        }
                        ImPlot::EndSubplots();
                    }

                    ImGui::SameLine();
                    if (ImGui::BeginChild("##Open Orders", ImVec2(0.25*windowSize.x, 0.45*windowSize.y), true)) {
                        std::vector<ats::Order> open_orders = m_open_orders[data.ticker];
                        // display orders in a table
                        if (!open_orders.empty()) {
                            ImGui::Columns(6, "OpenOrdersTable", true);
                            ImGui::Text("Type");
                            ImGui::NextColumn();
                            ImGui::Text("Symbol");
                            ImGui::NextColumn();
                            ImGui::Text("Side");
                            ImGui::NextColumn();
                            ImGui::Text("Quantity");
                            ImGui::NextColumn();
                            ImGui::Text("Price");
                            ImGui::NextColumn();
                            ImGui::Text("Time");
                            ImGui::NextColumn();
                            ImGui::Separator();
                            for (const auto &order: open_orders) {
                                ImGui::Text("%s", ats::OrderTypeToString(order.type).c_str());
                                ImGui::NextColumn();
                                ImGui::Text("%s", order.symbol.c_str());
                                ImGui::NextColumn();
                                if (order.side == ats::BUY)
                                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                                else ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
                                ImGui::Text("%s", order.side == ats::Side::BUY ? "BUY" : "SELL");
                                ImGui::PopStyleColor();
                                ImGui::NextColumn();
                                ImGui::Text("%f", order.quantity);
                                ImGui::NextColumn();
                                ImGui::Text("%f", order.price);
                                ImGui::NextColumn();
                                ImGui::Text("%s", ctime(&order.time));
                                ImGui::NextColumn();
                            }
                            ImGui::Columns(1);
                        } else {
                            ImGui::Text("No open orders.");
                        }
                        ImGui::EndChild();
                    }
                    ImGui::SameLine();
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 0.45*windowSize.y);
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 0.25*windowSize.x);
                    if (ImGui::BeginChild("##Balances", ImVec2(0.25*windowSize.x, 0.45*windowSize.y), true)) {
                        auto &b = m_balances;
                        if (!b.empty()) {
                            ImGui::Columns(2, "BalancesTable", true);
                            ImGui::Text("Asset");
                            ImGui::NextColumn();
                            ImGui::Text("Balance");
                            ImGui::NextColumn();
                            ImGui::Separator();
                            for (const auto &balance: b) {
                                ImGui::Text("%s", balance.first.c_str());
                                ImGui::NextColumn();
                                ImGui::Text("%f", balance.second);
                                ImGui::NextColumn();
                            }
                            ImGui::Columns(1);
                        } else {
                            ImGui::Text("No balances.");
                        }
                        ImGui::EndChild();
                    }


                    ImGui::EndTabItem();
                }
            }

            ImGui::EndTabBar();
        }

        ImGui::End();

    }

    BinanceAPI m_api;
    std::map<std::string, TickerData> m_ticker_data;
    std::map<std::string, std::vector<ats::Order>> m_open_orders;
    std::vector<std::pair<std::string, double>> m_balances;
    std::thread t;
    std::mutex m_data_mutex;

    ImBinance(std::string title, int w, int h, int argc, const char *argv[], ats::BinanceExchangeManager &ems) : App(
            title, w, h, argc, argv),
                                                                                                                 m_api(ems) {
    }

    ~ImBinance() {
        if (t.joinable())
            t.join();
    }
};

#endif