#include <string.h>
#include <stdlib.h>
#include <QString>

/**
 *
 * Returns a localised timestring
 *
 * Return value: The time string, e.g. "2 hours 3 minutes"
 **/
QString gpm_get_timestring (uint time_secs)
{
    char* timestring = NULL;
    int  hours;
    int  minutes;
    QString result;

    /* Add 0.5 to do rounding */
    minutes = (int) ( ( time_secs / 60.0 ) + 0.5 );

    if (minutes == 0) {
        result = (("Unknown time"));
        return result;
    }

    if (minutes < 60) {
        result.sprintf( "%i minute",minutes);
        return timestring;
    }

    hours = minutes / 60;
    minutes = minutes % 60;

    if (minutes == 0)
    {
        result.sprintf ("%i hour",hours);
    }
    else
    {
        result.sprintf (("%i %s %i %s"),hours, "hours",minutes, "minutes");
    }
    return result;
}


