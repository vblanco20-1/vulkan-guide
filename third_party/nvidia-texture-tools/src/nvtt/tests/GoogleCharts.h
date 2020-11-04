
#include "nvmath/Vector.inl"
#include "nvcore/StrLib.h"

namespace nv {

struct GooglePointSet {
    GooglePointSet();
    Vector2 min, max;
    Array<Vector2> points;
    const char * legend;
    Vector3 lineColor;
    int lineWidth;
    bool dashed;
    int lineSegmentLength;
    int blankSegmentLength;
};

struct GoogleLineChart {
    GoogleLineChart();
    void autoScale();
    void build(nv::StringBuilder & builder) const;

    const char * title;
    int width;
    int height;

    int leftMargin;
    int rightMargin;
    int topMargin;
    int bottomMargin;

    int legendPosition;

    Array<GooglePointSet> pointSetArray;
};



GooglePointSet::GooglePointSet()
{
    min.x = 0;
    min.y = 0;
    max.x = 1;
    max.x = 1;
    legend = NULL;
    lineColor = Vector3(1, 0, 0);
    lineWidth = 2;
    dashed = false;
    lineSegmentLength = 4;
    blankSegmentLength = 1;
}


GoogleLineChart::GoogleLineChart()
{
    title = NULL;
    width = 440;
    height = 220;

    leftMargin = 5;
    rightMargin = 5;
    topMargin = 5;
    bottomMargin = 25;

    legendPosition = 3; // bottom
}

void GoogleLineChart::autoScale()
{
    Vector2 minimum, maximum;
    foreach(i, pointSetArray) {
        foreach(p, pointSetArray[i].points) {
            Vector2 point = pointSetArray[i].points[p];
            minimum = min(minimum, point);
            maximum = max(maximum, point);
        }
    }

    foreach(i, pointSetArray) {
        pointSetArray[i].min = minimum;
        pointSetArray[i].max = maximum;
    }
}

void GoogleLineChart::build(nv::StringBuilder & builder) const
{
    const uint lineCount = pointSetArray.count();

    builder.reset();

    // Start LineChartXY.
    builder.copy("http://chart.apis.google.com/chart?cht=lxy");

    // Size.
    builder.appendFormat("&chs=%dx%d", width, height);

    // Title.
    if (title != NULL) builder.appendFormat("&chtt=%s", title);

    // Margins.
    builder.appendFormat("&chma=%d,%d,%d,%d", leftMargin, rightMargin, topMargin, bottomMargin);

    // Legend position.
    builder.appendFormat("&chdlp=%c", "lrtb"[legendPosition]);

    // Line colors.
    builder.append("&chco=");
    for (uint i = 0; i < lineCount; i++) {
        const GooglePointSet & set = pointSetArray[i];
        builder.appendFormat("%.2X%.2X%.2X", int(255 * set.lineColor.x), int(255 * set.lineColor.y), int(255 * set.lineColor.z));
        if (i != lineCount-1) builder.append(",");
    }

    // Legends.
    builder.append("&chdl=");
    for (uint i = 0; i < lineCount; i++) {
        const GooglePointSet & set = pointSetArray[i];
        if (set.legend != NULL) builder.append(set.legend);
        if (i != lineCount-1) builder.append("|");
    }

    // Line format.
    builder.append("&chls=");
    for (uint i = 0; i < lineCount; i++) {
        const GooglePointSet & set = pointSetArray[i];
        builder.appendFormat("%d", set.lineWidth);
        if (set.dashed) builder.appendFormat(",%d,%d", set.lineSegmentLength, set.blankSegmentLength);
        if (i != lineCount-1) builder.append("|");
    }

    // Scaling.
    builder.append("&chds=");

    for (uint i = 0; i < lineCount; i++) {
        const GooglePointSet & set = pointSetArray[i];
        builder.appendFormat("%f,%f,%f,%f", set.min.x, set.max.x, set.min.y, set.max.y);
        if (i != lineCount-1) builder.append(",");
    }

    // Data.
    builder.append("&chd=t:");
    for (uint i = 0; i < lineCount; i++) {
        const GooglePointSet & set = pointSetArray[i];

        const uint pointCount = set.points.count();
        /*for (uint p = 0; p < pointCount; p++) {
            builder.appendFormat("%f", set.points[p].x);
            if (p != pointCount-1) builder.append(",");
        }*/
        builder.append("-1");

        builder.append("|");

        for (uint p = 0; p < pointCount; p++) {
            builder.appendFormat("%f", set.points[p].y);
            if (p != pointCount-1) builder.append(",");
        }

        if (i != lineCount-1) builder.append("|");
    }
}

} // nv namespace
