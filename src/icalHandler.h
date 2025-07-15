#pragma once

#include <string>
#include <set>
#include <uICAL.h>



class IcalEventHandler {
    public:
        virtual String getEventName() { return ""; };
        virtual void onEventStart(uICAL::CalendarEntry_ptr evt) {
            Serial.print("Non-overwritten onEventStart for event ");
            Serial.println(this->getEventName());
        };

        virtual void onEventEnd(uICAL::CalendarEntry_ptr evt) {
            Serial.print("Non-overwritten onEventEnd for event ");
            Serial.println(this->getEventName());
        };
};


class RelayHandler : public IcalEventHandler {
    protected:
        String name;
        int pin;
        bool inverted;

    public:
        RelayHandler(String eventName, int pinId, bool invertedState=false) : name(eventName), pin(pinId), inverted(invertedState) {
            pinMode(pinId, OUTPUT);
            digitalWrite(this->pin, inverted); // Default state to off
        }

        String getEventName() override {
            return name;
        }

        void onEventStart(uICAL::CalendarEntry_ptr evt) override {
            digitalWrite(this->pin, !inverted);
        }

        void onEventEnd(uICAL::CalendarEntry_ptr evt) override {
            digitalWrite(this->pin, inverted);
        }
};

class RelayHandlerMaxTimed : public RelayHandler {
    protected:
        int millisecondsOn;

    public:
        RelayHandlerMaxTimed(String eventName, int pinId, int millisecondsOn, bool invertedState=false) : RelayHandler(eventName, pinId, invertedState), millisecondsOn(millisecondsOn) {

        }

        void onEventStart(uICAL::CalendarEntry_ptr evt) override {
            digitalWrite(this->pin, !inverted);
            delay(this->millisecondsOn);
            digitalWrite(this->pin, inverted);
        }

};


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

        void print() {
            Serial.print(this->unixtime);
            Serial.print(this->startingRatherThanEnding ? " - STARTING: " :  " - ENDING:   " );
            Serial.print(this->event->summary());
        }
};

struct SortByUnixtime {
    bool operator ()(const EventLogItem& lhs, const EventLogItem& rhs) const // Return true if lhs is lesser than rhs
    {
        int timeDiff = lhs.unixtime - rhs.unixtime;
        if (timeDiff != 0)
            return timeDiff < 0; // Lowest unixtime first

        if(lhs.event == rhs.event) {
            // Start before finalizing
            return lhs.startingRatherThanEnding && !rhs.startingRatherThanEnding;
        }

        // Finalize tasks before starting new ones
        return !lhs.startingRatherThanEnding && rhs.startingRatherThanEnding;
    }
};


class IcalHandler {
    private:
        uICAL::CalendarIter_ptr icalEventIterator;
        std::set<EventLogItem, SortByUnixtime> eventLog;

        void insertEventIntoLog(uICAL::CalendarEntry_ptr event) {
            this->eventLog.insert(EventLogItem(event->start(), event, true));
            this->eventLog.insert(EventLogItem(event->end(), event, false));

            if(this->verbose)
                printEventQueue();
        }


        void consumeNextEventFromIcalStream() {
            this->icalEventIterator->next();
            uICAL::CalendarEntry_ptr entry = icalEventIterator->current();

            this->insertEventIntoLog(entry);
        }


        std::map<String, IcalEventHandler*> eventHandlers;
        void processLogItem(EventLogItem &logItem) {
            uICAL::CalendarEntry_ptr event = logItem.event;
            bool startingRatherThanEnding = logItem.startingRatherThanEnding;

            String eventName = event->summary();
            IcalEventHandler* eventHandler;
            try {
                eventHandler = this->eventHandlers.at(eventName);
            }
            catch(const std::out_of_range& ex) {
                if(this->verbose) {
                    Serial.print("Unhandled event: \"");
                    Serial.print(eventName);
                    Serial.println("\"");
                }
                return;
            }

            if(startingRatherThanEnding) {
                if(this->verbose) {
                    Serial.print("Starting event ");
                    Serial.println(eventName);
                }
                eventHandler->onEventStart(event);
            }
            else {
                if(this->verbose) {
                    Serial.print("Finishing event ");
                    Serial.println(eventName);
                }
                eventHandler->onEventEnd(event);
            }
        }


    public:
        bool verbose;
        IcalHandler(uICAL::CalendarIter_ptr icalEventIterator, bool verbose=false) : icalEventIterator(icalEventIterator), verbose(verbose) {
            this->consumeNextEventFromIcalStream();
        }

        void printEventQueue() {
            Serial.println("Event Queue:");

            for(EventLogItem item : this->eventLog) {
                Serial.print("\t");
                item.print();
                Serial.println();
            }
            Serial.println();
        }
        

        void registerEventHandler(IcalEventHandler* handler) {
            if(this->verbose) {
                Serial.print("Registering event handler: \"");
                Serial.print(handler->getEventName());
                Serial.println("\"");
            }

            this->eventHandlers[handler->getEventName()] = handler;
        }


        bool hasNextEvent() {
            return !this->eventLog.empty();
        }


        time_t getTimeOfNextEvent() {
            return (*(this->eventLog.begin())).unixtime;
        }


        void processEventsUntil(time_t time) {
            for(EventLogItem item : this->eventLog) {
                if(item.unixtime > time) break;
            
                if(item.startingRatherThanEnding) { // Always keep at least 1 upcoming event in the log
                    this->consumeNextEventFromIcalStream();
                }

                this->processLogItem(item);
                this->eventLog.erase(item);
            }
            if(this->verbose)
                printEventQueue();
        }
};


