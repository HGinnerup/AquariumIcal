#pragma once

#include "common.h"
#include <set>
#include <uICAL.h>

#include "logger.h"
#include "icalEventHandlers/icalEventHandler.h"

time_t uicalDateTimeToUnixtime(uICAL::DateTime icalTime) {
    return (time_t)(icalTime - uICAL::DateTime(0)).totalSeconds();
};

class EventLogItem {
public:
    time_t unixtime;
    uICAL::CalendarEntry_ptr event;
    bool startingRatherThanEnding;

    EventLogItem(time_t unixtime, uICAL::CalendarEntry_ptr event, bool startingRatherThanEnding) : unixtime(unixtime), event(event), startingRatherThanEnding(startingRatherThanEnding) {

    }

    EventLogItem(uICAL::DateTime icalTime, uICAL::CalendarEntry_ptr event, bool startingRatherThanEnding) : event(event), startingRatherThanEnding(startingRatherThanEnding) {
        this->unixtime = uicalDateTimeToUnixtime(icalTime);
    }

    static int compare(const EventLogItem a, const EventLogItem b) {
        return a.unixtime - b.unixtime;
    }

    String toString() const {
        char result[40];
        sprintf(result, "%u - %s %s",
            this->unixtime,
            this->startingRatherThanEnding ? "STARTING:" : "ENDING: ",
            event->summary().c_str()
        );

        return result;
    }

};

struct SortByUnixtime {
    bool operator ()(const EventLogItem& lhs, const EventLogItem& rhs) const // Return true if lhs is lesser than rhs
    {
        int timeDiff = lhs.unixtime - rhs.unixtime;
        if (timeDiff != 0)
            return timeDiff < 0; // Lowest unixtime first

        if (lhs.event == rhs.event) {
            // Start before finalizing
            return lhs.startingRatherThanEnding && !rhs.startingRatherThanEnding;
        }

        // Finalize tasks before starting new ones
        return !lhs.startingRatherThanEnding && rhs.startingRatherThanEnding;
    }
};


class IcalHandler {
protected:
    Logger* logger;
private:
    uICAL::CalendarIter_ptr icalEventIterator;
    std::set<EventLogItem, SortByUnixtime> eventLog;

    void insertEventIntoLog(uICAL::CalendarEntry_ptr event) {
        this->eventLog.insert(EventLogItem(event->start(), event, true));
        this->eventLog.insert(EventLogItem(event->end(), event, false));
        printEventQueue(LogLevel::DEBUG);
    }


    void consumeNextEventFromIcalStream() {
        this->icalEventIterator->next();
        uICAL::CalendarEntry_ptr entry = icalEventIterator->current();

        this->insertEventIntoLog(entry);
    }


    std::map<String, IcalEventHandler*> eventHandlers;
    void processLogItem(EventLogItem& logItem) {
        uICAL::CalendarEntry_ptr event = logItem.event;
        bool startingRatherThanEnding = logItem.startingRatherThanEnding;

        String eventName = event->summary();
        IcalEventHandler* eventHandler;
        
        try {
            eventHandler = this->eventHandlers.at(eventName);
        }
        catch (const std::out_of_range& ex) {
            this->logger->warn("Unhandled event: \"" + eventName + "\"");
            return;
        }

        if (startingRatherThanEnding) {
            this->logger->info("Starting event: \"" + eventName + "\"");
            eventHandler->onEventStart(event);
        }
        else {
            this->logger->info("Finishing event: \"" + eventName + "\"");
            eventHandler->onEventEnd(event);
        }
    }


public:
    IcalHandler(uICAL::CalendarIter_ptr icalEventIterator, Logger* logger = &Logger::getInstance()) : icalEventIterator(icalEventIterator), logger(logger) {
        this->consumeNextEventFromIcalStream();
    }

    void printEventQueue(LogLevel lvl = LogLevel::INFO) {
        this->logger->log(lvl, "Event Queue:");

        for (EventLogItem item : this->eventLog) {
            this->logger->log(lvl, "\t" + item.toString());
        }
        this->logger->log(lvl, "");
    }


    void registerEventHandler(IcalEventHandler* handler) {
        this->logger->info("Registering event handler: \"" + handler->getEventName() + "\"");

        this->eventHandlers[handler->getEventName()] = handler;
    }


    bool hasNextEvent() {
        return !this->eventLog.empty();
    }


    time_t getTimeOfNextEvent() {
        return (*(this->eventLog.begin())).unixtime;
    }


    void processEventsUntil(time_t time) {
        for (EventLogItem item : this->eventLog) {
            if (item.unixtime > time) break;

            if (item.startingRatherThanEnding) { // Always keep at least 1 upcoming event in the log
                this->consumeNextEventFromIcalStream();
            }

            this->processLogItem(item);
            this->eventLog.erase(item);
        }

        printEventQueue();
    }
};
