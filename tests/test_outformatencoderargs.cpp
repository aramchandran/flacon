/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Flacon - audio File Encoder
 * https://github.com/flacon/flacon
 *
 * Copyright: 2017
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * END_COMMON_COPYRIGHT_HEADER */


#include "testflacon.h"
#include "tools.h"
#include "outformat.h"
#include "disk.h"

#include <QTest>
#include <QString>
#include <QDebug>


/************************************************

 ************************************************/
void TestFlacon::testOutFormatEncoderArgs()
{
    QFETCH(QString, formatId);
    QFETCH(SettingsValues, config);
    QFETCH(QString, expected);

    applySettings(config);

    foreach (OutFormat *format, OutFormat::allFormats())
    {
        if (format->id() != formatId)
            continue;

        Disk *disk = standardDisk();
        QStringList args = format->encoderArgs(disk->track(0), "OutFile.wav");

        QString result = args.join(" ");
        if (result != expected)
        {
            QString msg = QString("Compared values are not the same\n   Format   %1\n   Actual:   %2\n   Expected: %3").arg(
                        formatId,
                        result,
                        expected);
            QFAIL(msg.toLocal8Bit());
        }
        return;
    }

    FAIL(QString("Unknown format \"%1\"").arg(formatId).toLocal8Bit());
}


/************************************************

 ************************************************/
void TestFlacon::testOutFormatEncoderArgs_data()
{
    QTest::addColumn<QString>("formatId");
    QTest::addColumn<SettingsValues>("config");
    QTest::addColumn<QString>("expected");

    SettingsValues cfg;

    //*******************************************
    // FLAC
    //*******************************************
    cfg.clear();
    cfg.insert("Programs/flac",     "/opt/flac");
    cfg.insert("Flac/Compression",  5);
    cfg.insert("Flac/ReplayGain",   "Disable");

    QTest::newRow("Flac_1")
            << "FLAC"
            << cfg
            << "/opt/flac --force --silent "
               "--compression-level-5 "
               "--tag artist=Artist --tag album=Album --tag genre=Genre --tag date=2013 --tag title=Song01 --tag comment=ExactAudioCopy v0.99pb4 "
               "--tag discId=123456789 --tag TRACKNUMBER=1 --tag TOTALTRACKS=4 --tag TRACKTOTAL=4 - -o OutFile.wav";

    //*******************************************
    cfg.clear();
    cfg.insert("Programs/flac",     "/opt/flac");
    cfg.insert("Flac/Compression",  1);
    cfg.insert("Flac/ReplayGain",   "Disable");

    QTest::newRow("Flac_2")
            << "FLAC"
            << cfg
            << "/opt/flac --force --silent "
               "--compression-level-1 "
               "--tag artist=Artist --tag album=Album --tag genre=Genre --tag date=2013 --tag title=Song01 --tag comment=ExactAudioCopy v0.99pb4 "
               "--tag discId=123456789 --tag TRACKNUMBER=1 --tag TOTALTRACKS=4 --tag TRACKTOTAL=4 - -o OutFile.wav";


    //*******************************************
    // AAC
    //*******************************************
    cfg.clear();
    cfg.insert("Programs/faac",  "/opt/faac");
    cfg.insert("Aac/UseQuality", true);
    cfg.insert("Aac/Quality",    500);

    QTest::newRow("AAC_1")
            << "AAC"
            << cfg
            << "/opt/faac -w "
               "-q 500 "
               "--artist Artist --title Song01 --genre Genre --album Album --track 1/4 --year 2013 --comment ExactAudioCopy v0.99pb4 -o OutFile.wav -";


    //*******************************************
    cfg.clear();
    cfg.insert("Programs/faac",  "/opt/faac");
    cfg.insert("Aac/UseQuality", true);
    cfg.insert("Aac/Quality",    10);

    QTest::newRow("AAC_2")
            << "AAC"
            << cfg
            << "/opt/faac -w "
               "-q 10 "
               "--artist Artist --title Song01 --genre Genre --album Album --track 1/4 --year 2013 --comment ExactAudioCopy v0.99pb4 -o OutFile.wav -";

    //*******************************************
    cfg.clear();
    cfg.insert("Programs/faac",  "/opt/faac");
    cfg.insert("Aac/UseQuality", false);
    cfg.insert("Aac/Quality",    500);
    cfg.insert("Aac/Bitrate",    64);

    QTest::newRow("AAC_3")
            << "AAC"
            << cfg
            << "/opt/faac -w "
               "-b 64 "
               "--artist Artist --title Song01 --genre Genre --album Album --track 1/4 --year 2013 --comment ExactAudioCopy v0.99pb4 -o OutFile.wav -";


    //*******************************************
    // MP3
    //*******************************************
    cfg.clear();
    cfg.insert("Programs/lame",  "/opt/lame");
    cfg.insert("Mp3/Preset",     "vbrMedium");
    cfg.insert("Aac/Quality",    500);

    QTest::newRow("MP3_vbrMedium")
            << "MP3"
            << cfg
            << "/opt/lame --silent "
               "--preset medium "
               "--noreplaygain --add-id3v2 --ta Artist --tl Album --tg Genre --ty 2013 --tt Song01 --tc ExactAudioCopy v0.99pb4 --tn 1/4 - OutFile.wav";


    //*******************************************
    cfg.insert("Mp3/Preset",     "vbrStandard");

    QTest::newRow("MP3_vbrStandard")
            << "MP3"
            << cfg
            << "/opt/lame "
               "--silent "
               "--preset standard "
               "--noreplaygain --add-id3v2 --ta Artist --tl Album --tg Genre --ty 2013 --tt Song01 --tc ExactAudioCopy v0.99pb4 --tn 1/4 - OutFile.wav";



    //*******************************************
    cfg.insert("Mp3/Preset",     "vbrStandardFast");

    QTest::newRow("MP3_vbrStandardFast")
            << "MP3"
            << cfg
            << "/opt/lame --silent "
               "--preset fast standard "
               "--noreplaygain --add-id3v2 --ta Artist --tl Album --tg Genre --ty 2013 --tt Song01 --tc ExactAudioCopy v0.99pb4 --tn 1/4 - OutFile.wav";


    //*******************************************
    cfg.insert("Mp3/Preset",     "vbrExtreme");

    QTest::newRow("MP3_vbrExtreme")
            << "MP3"
            << cfg
            << "/opt/lame --silent "
               "--preset extreme "
               "--noreplaygain --add-id3v2 --ta Artist --tl Album --tg Genre --ty 2013 --tt Song01 --tc ExactAudioCopy v0.99pb4 --tn 1/4 - OutFile.wav";

    //*******************************************
    cfg.insert("Mp3/Preset",     "vbrExtremeFast");

    QTest::newRow("MP3_vbrExtremeFast")
            << "MP3"
            << cfg
            << "/opt/lame --silent "
               "--preset fast extreme "
               "--noreplaygain --add-id3v2 --ta Artist --tl Album --tg Genre --ty 2013 --tt Song01 --tc ExactAudioCopy v0.99pb4 --tn 1/4 - OutFile.wav";


    //*******************************************
    cfg.insert("Mp3/Preset",     "cbrInsane");

    QTest::newRow("MP3_cbrInsane")
            << "MP3"
            << cfg
            << "/opt/lame --silent "
               "--preset insane "
               "--noreplaygain --add-id3v2 --ta Artist --tl Album --tg Genre --ty 2013 --tt Song01 --tc ExactAudioCopy v0.99pb4 --tn 1/4 - OutFile.wav";

    //*******************************************
    cfg.insert("Mp3/Preset",     "cbrKbps");
    cfg.insert("Mp3/Bitrate",    64);

    QTest::newRow("MP3_cbrKbps64")
            << "MP3"
            << cfg
            << "/opt/lame --silent "
               "--preset cbr 64 "
               "--noreplaygain --add-id3v2 --ta Artist --tl Album --tg Genre --ty 2013 --tt Song01 --tc ExactAudioCopy v0.99pb4 --tn 1/4 - OutFile.wav";

    //*******************************************
    cfg.insert("Mp3/Preset",     "cbrKbps");
    cfg.insert("Mp3/Bitrate",    128);

    QTest::newRow("MP3_cbrKbps128")
            << "MP3"
            << cfg
            << "/opt/lame --silent "
               "--preset cbr 128 "
               "--noreplaygain --add-id3v2 --ta Artist --tl Album --tg Genre --ty 2013 --tt Song01 --tc ExactAudioCopy v0.99pb4 --tn 1/4 - OutFile.wav";


    //*******************************************
    cfg.insert("Mp3/Preset",     "abrKbps");
    cfg.insert("Mp3/Bitrate",    64);

    QTest::newRow("MP3_abrKbps64")
            << "MP3"
            << cfg
            << "/opt/lame --silent "
               "--preset 64 "
               "--noreplaygain --add-id3v2 --ta Artist --tl Album --tg Genre --ty 2013 --tt Song01 --tc ExactAudioCopy v0.99pb4 --tn 1/4 - OutFile.wav";

    //*******************************************
    cfg.insert("Mp3/Preset",     "abrKbps");
    cfg.insert("Mp3/Bitrate",    128);

    QTest::newRow("MP3_abrKbps128")
            << "MP3"
            << cfg
            << "/opt/lame --silent "
               "--preset 128 "
               "--noreplaygain --add-id3v2 --ta Artist --tl Album --tg Genre --ty 2013 --tt Song01 --tc ExactAudioCopy v0.99pb4 --tn 1/4 - OutFile.wav";


    //*******************************************
    cfg.insert("Mp3/Preset",     "vbrQuality");
    cfg.insert("Mp3/Quality",    0);

    QTest::newRow("MP3_vbrQuality0")
            << "MP3"
            << cfg
            << "/opt/lame --silent "
               "-V 9 "
               "--noreplaygain --add-id3v2 --ta Artist --tl Album --tg Genre --ty 2013 --tt Song01 --tc ExactAudioCopy v0.99pb4 --tn 1/4 - OutFile.wav";

    //*******************************************
    cfg.insert("Mp3/Preset",     "vbrQuality");
    cfg.insert("Mp3/Quality",    4);

    QTest::newRow("MP3_vbrQuality4")
            << "MP3"
            << cfg
            << "/opt/lame --silent "
               "-V 5 "
               "--noreplaygain --add-id3v2 --ta Artist --tl Album --tg Genre --ty 2013 --tt Song01 --tc ExactAudioCopy v0.99pb4 --tn 1/4 - OutFile.wav";

    //*******************************************
    cfg.insert("Mp3/Preset",     "vbrQuality");
    cfg.insert("Mp3/Quality",    9);

    QTest::newRow("MP3_vbrQuality9")
            << "MP3"
            << cfg
            << "/opt/lame --silent "
               "-V 0 "
               "--noreplaygain --add-id3v2 --ta Artist --tl Album --tg Genre --ty 2013 --tt Song01 --tc ExactAudioCopy v0.99pb4 --tn 1/4 - OutFile.wav";



    //*******************************************
    // Ogg
    //*******************************************
    cfg.clear();
    cfg.insert("Programs/oggenc",   "/opt/oggenc");
    cfg.insert("Ogg/UseQuality",    true);
    cfg.insert("Ogg/Quality",       5);
    cfg.insert("Ogg/MinBitrate",    "");
    cfg.insert("Ogg/NormBitrate",   "");
    cfg.insert("Ogg/MaxBitrate",    "");

    QTest::newRow("Ogg Quality 5")
            << "OGG"
            << cfg
            << "/opt/oggenc --quiet "
               "-q 5 "
               "--artist Artist --album Album --genre Genre --date 2013 --title Song01 --comment COMMENT=ExactAudioCopy v0.99pb4 "
               "--comment DISCID=123456789 --tracknum 1 --comment TOTALTRACKS=4 --comment TRACKTOTAL=4 -o OutFile.wav -";


    //*******************************************
    cfg.insert("Ogg/Quality",       10);

    QTest::newRow("Ogg Quality 10")
            << "OGG"
            << cfg
            << "/opt/oggenc --quiet "
               "-q 10 "
               "--artist Artist --album Album --genre Genre --date 2013 --title Song01 --comment COMMENT=ExactAudioCopy v0.99pb4 "
               "--comment DISCID=123456789 --tracknum 1 --comment TOTALTRACKS=4 --comment TRACKTOTAL=4 -o OutFile.wav -";


    //*******************************************
    cfg.insert("Programs/oggenc",   "/opt/oggenc");
    cfg.insert("Ogg/UseQuality",    false);
    cfg.insert("Ogg/MinBitrate",    "");
    cfg.insert("Ogg/NormBitrate",   "");
    cfg.insert("Ogg/MaxBitrate",    "");

    QTest::newRow("Ogg Bitrate 0 0 0")
            << "OGG"
            << cfg
            << "/opt/oggenc --quiet "
               "--artist Artist --album Album --genre Genre --date 2013 --title Song01 --comment COMMENT=ExactAudioCopy v0.99pb4 "
               "--comment DISCID=123456789 --tracknum 1 --comment TOTALTRACKS=4 --comment TRACKTOTAL=4 -o OutFile.wav -";

    //*******************************************
    cfg.insert("Programs/oggenc",   "/opt/oggenc");
    cfg.insert("Ogg/UseQuality",    false);
    cfg.insert("Ogg/MinBitrate",    64);
    cfg.insert("Ogg/NormBitrate",   128);
    cfg.insert("Ogg/MaxBitrate",    350);

    QTest::newRow("Ogg Bitrate 64 128 350")
            << "OGG"
            << cfg
            << "/opt/oggenc --quiet "
               "-b 128 -m 64 -M 350 "
               "--artist Artist --album Album --genre Genre --date 2013 --title Song01 --comment COMMENT=ExactAudioCopy v0.99pb4 "
               "--comment DISCID=123456789 --tracknum 1 --comment TOTALTRACKS=4 --comment TRACKTOTAL=4 -o OutFile.wav -";



    //*******************************************
    // WavPack
    //*******************************************
    cfg.clear();
    cfg.insert("Programs/wavpack",  "/opt/wavpack");
    cfg.insert("WV/Compression",   0);

    QTest::newRow("WavPack Quality 0")
            << "WV"
            << cfg
            << "/opt/wavpack -q "
               "-f "
               "-w Artist=Artist -w Album=Album -w Genre=Genre -w Year=2013 -w Title=Song01 -w DiscId=123456789 -w Comment=ExactAudioCopy v0.99pb4 -w Track=1/4 - -o OutFile.wav";


    //*******************************************
    cfg.insert("WV/Compression",   1);

    QTest::newRow("WavPack Quality 1")
            << "WV"
            << cfg
            << "/opt/wavpack -q "
               "-h "
               "-w Artist=Artist -w Album=Album -w Genre=Genre -w Year=2013 -w Title=Song01 -w DiscId=123456789 -w Comment=ExactAudioCopy v0.99pb4 -w Track=1/4 - -o OutFile.wav";


    //*******************************************
    cfg.insert("WV/Compression",   2);

    QTest::newRow("WavPack Quality 2")
            << "WV"
            << cfg
            << "/opt/wavpack -q "
               "-hh "
               "-w Artist=Artist -w Album=Album -w Genre=Genre -w Year=2013 -w Title=Song01 -w DiscId=123456789 -w Comment=ExactAudioCopy v0.99pb4 -w Track=1/4 - -o OutFile.wav";

}
