<?
include('config.php');
include('lastfm.php');
session_id('static');
session_start();


function cleanup($str)
{
    setlocale(LC_ALL, 'en_US.UTF8');
    $clean = iconv('UTF-8', 'ASCII//TRANSLIT', $str);
    $clean = preg_replace('/\s+/', ' ', $clean);
    return $clean;
}

$pauseString = " ... ";

function getColor()
{
    $colors = [];

    for ($i = 0; $i < 50; $i++) {
        $r = rand(128, 255); // Generate random values between 128 and 255 for brightness
        $g = rand(128, 255); // Generate random values between 128 and 255 for warmth
        $b = rand(0, 127); // Generate random values between 0 and 127 to keep the color warm

        $colors[] = " ($r,$g,$b) ";
    }
    return $colors[array_rand($colors)];
}

$output = "404";

switch ($_GET['screen']) {
    case 'custom':
        $file = "message.txt";
        $output = "";
        $output = @file_get_contents($file);
        break;
    case 'music':
        $output = $pauseString;
        $tracks = getLastSongs();
        $numberOfTracks = min(count($tracks), 2);
        for ($i = 0; $i < $numberOfTracks; $i++) {
            if ($i === 0) {
              //  $output .= getColor() . "[Music]" . $pauseString;
            }
            $track = $tracks[$i];
            $time = str_replace('Scrobbling', '', $track['time']);
            $output .= getColor() . $time . $pauseString;
            $output .= getColor() . $track['name'] . ' by ' . $track['artist'];
            $output .= $pauseString;

        }
        break;
    case 'joke':
        $currentSlogans = file_get_contents("slogans.txt");
        if(!$_SESSION["COUNT"]) {
            $_SESSION["COUNT"] = 0;
        }
        $currentSlogans = preg_split("/\r\n|\n|\r/", $currentSlogans);


        if($_SESSION["COUNT"] >= count($currentSlogans)-1) {
            $_SESSION["COUNT"] = 0;
        } else {
            $_SESSION["COUNT"]++;
        }
     
        $output = $pauseString;
            $output .= getColor() . $currentSlogans[$_SESSION["COUNT"]];
            $output .= $pauseString;
        break;
}

echo cleanup($output);