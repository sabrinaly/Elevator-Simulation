while (1)
{
    /*********  ELEVATOR GOING UP  **********/

    while (elevator_floor < last_requested_floor) //do we need to check for elevator_direction too??
    {
        for (i = 0; i < COMMAND_SIZE; i++)
        {
            // if passenger getting off (command/10 = 0) or getting on to go down (command/10 = 1)
            if (commands[i].valid && (elevator_floor == commands[i].command % 10) && (commands[i].command / 10 != 2))
                stopped_flag = 1;
        }
        if (stopped_flag)
        {
            stop_elevator(REQ_DOWN, EV_UP);
        }
        stopped_flag = 0;
        Sleep(1000);
        elevator_floor++;
        elevator_direction = 1;
        update_status();
    }

    /*********  ELEVATOR GOING DOWN  **********/

    while (elevator_floor > last_requested_floor)
    {
        for (i = 0; i < COMMAND_SIZE; i++)
        {
            // if passenger getting off (command/10 = 0) or getting on to go down (command/10 = 2)
            if (commands[i].valid && (elevator_floor == commands[i].command % 10) && (commands[i].command / 10 != 1))
                stopped_flag = 1;
        }
        if (stopped_flag)
        {
            stop_elevator(REQ_DOWN, EV_DOWN);
        }
        stopped_flag = 0;
        Sleep(1000);
        elevator_floor--;
        elevator_direction = 0;
        update_status();
    }

    /**================================================== *
		 * ==========  Elevator at target floor  ============ *
		 * ================================================== */

    int passenger_waiting = NOONE; // 0 = no one waiting, 1 = waiting and going same direction, 2 = waiting and going opposite direction
    if (elevator_floor == last_requested_floor)
    {
        for (i = 0; i < COMMAND_SIZE; i++)
        {
            if (commands[i].valid && (elevator_floor == commands[i].command % 10))
            {
                // passenger(s) waiting on this floor, going same direction
                // when they enter, last requested floor will be greater than the current one
                // new last_request_floor will be set in the other thread
                if ((elevator_direction && commands[i].command / 10) || (elevator_direction == 0 && commands[i].command / 10 == 2))
                {
                    passenger_waiting = SAME_DIR;
                    break;
                }
                // passenger waiting on this floor, but requested the opposite direction
                else if ((elevator_direction && commands[i].command / 10 == 2) || (elevator_direction == 0 && commands[i].command / 10))
                {
                    //still need passengers to get off, bottom part still needs to work
                    // after we need to signal the opposite direction
                    passenger_waiting = OPP_DIR;
                }
            }
        }

        /*********  NO PASSENGERS  **********/

        // if no passengers are waiting at this floor, then we set a new last_requested_floor
        int largest_age = 0;
        int largest_age_index = 0;
        if (passenger_waiting == NOONE)
        {
            // opens door to let passengers out first
            if (elevator_direction)
                stop_elevator(NOONE, EV_UP);
            else
                stop_elevator(NOONE, EV_DOWN);
            //poll for new command
            while (largest_age == 0)
            {
                for (i = 0; i < COMMAND_SIZE; i++)
                {
                    if (commands[i].valid && commands[i].age > largest_age)
                    {
                        largest_age = commands[i].age;
                        largest_age_index = i;
                    }
                }
                // if largest age is 0, that means there are no valid commands in the list and elevator should be idle
                // should it go back to middle floor?
                if (largest_age != 0)
                {
                    last_requested_floor = commands[largest_age_index].command % 10;
                    commands[largest_age_index].valid = 0;
                    if (last_requested_floor > elevator_floor)
                    {
                        elevator_direction = 1;
                        Sleep(1000);
                        elevator_floor++;
                        update_status();
                    }
                    else if (last_requested_floor < elevator_floor)
                    {
                        elevator_direction = 0;
                        Sleep(1000);
                        elevator_floor--;
                        update_status();
                    }
                    // what if last_requested_floor == elevator_floor
                    // after a while, passenger arrives on the same floor
                    // open the door for them, then go to the direction they want
                    else
                    {
                        // passenger arrives on elevator's floor, presses up
                        if (commands[largest_age_index].command / 10)
                        {
                            elevator_direction = 1;
                            stop_elevator(REQ_UP, EV_UP);
                            Sleep(1000);
                            elevator_floor++;
                            update_status();
                        }
                        else // command/10 == 2
                        {
                            elevator_direction = 0;
                            stop_elevator(REQ_DOWN, EV_DOWN);
                            Sleep(1000);
                            elevator_floor--;
                            update_status();
                        }
                    }
                }
            }
        }

        /*********  PASSENGER GOING SAME DIR  **********/

        else if (passenger_waiting == SAME_DIR)
        {
            if (elevator_direction)
            { // elevator went up, passenger requesting up
                stop_elevator(REQ_UP, EV_UP);
                Sleep(1000);
                elevator_floor++;
                update_status();
            }
            else
            { // elevator went down, passenger requesting down
                stop_elevator(REQ_DOWN, EV_DOWN);
                Sleep(1000);
                elevator_floor--;
                update_status();
            }
        }

        /*********  PASSENGER GOING OPP DIR  **********/

        else if (passenger_waiting == OPP_DIR)
        { // passenger_waiting == 2
            if (elevator_direction)
            { // elevator went up, passenger requesting down
                elevator_direction = 0;
                EV1_UP_SIGNAL(); // for passengers getting off
                EV1_UP_RESET();
                //elevator going down now and passengers getting on to go down
                stop_elevator(REQ_DOWN, EV_DOWN);
                Sleep(1000);
                elevator_floor--;
                update_status();
            }
            else
            { // elevator went down, passenger requesting up
                elevator_direction = 1;
                EV1_DW_SIGNAL(); // for passengers getting off
                EV1_DW_RESET();
                //elevator going up now and passengers getting on to go up
                stop_elevator(REQ_UP, EV_UP);
                Sleep(1000);
                elevator_floor++;
                update_status();
            }
        }
    }
}

//clears command that have been consumed (e.g passenger got off, passenger got on)
void clear_command(int requested_direction)
{
    if (requested_direction == 0)
    {
        for (int i = 0; i < COMMAND_SIZE; i++)
        {
            if (commands[i].valid && (elevator_floor == commands[i].command % 10) && (commands[i].command / 10 == requested_direction))
            {
                commands[i].valid = 0;
            }
        }
    }
    else
    {
        for (int i = 0; i < COMMAND_SIZE; i++)
        {
            if (commands[i].valid && (elevator_floor == commands[i].command % 10) && (commands[i].command / 10 != requested_direction))
            {
                commands[i].valid = 0;
            }
        }
    }
}
