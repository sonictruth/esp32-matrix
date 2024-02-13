<?

function getLastSongs()
{
	global $xpath;
	$user = 'stereobarcelona';
	$page = @file_get_contents('https://www.last.fm/user/' . $user);

	$dom = new DomDocument();
	libxml_use_internal_errors(true);
	@$dom->loadHTML($page);
	$xpath = new DOMXpath($dom);
	$rows = $xpath->query('.//*[@id="recent-tracks-section"]//*[contains(concat(" ",normalize-space(@class)," ")," chartlist-row ")]');

	$tracks = [];


	function getTextFromNode($className, $node)
	{
		global $xpath;
		$textEl = $xpath->query(".//*[contains(concat(' ', normalize-space(@class), ' '), ' " . $className . " ')]", $node);

		if ($textEl) {
			return trim($textEl[0]->nodeValue);
		}
		return "";
	}

	if ($rows) {
		foreach ($rows as $row) {
			$imgEl = $xpath->query(".//img", $row);
			$imgUrl = $imgEl[0]->getAttribute('src');
			$name = getTextFromNode("chartlist-name", $row);
			$artist = getTextFromNode("chartlist-artist", $row);
			$time = getTextFromNode("chartlist-timestamp", $row);
			if(strpos($time, ":")) {
				$time = " - ";
			}
			
			if (strpos($time, "now")) {
				$now = true;
			} else {
				$now = false;
			}
			array_push($tracks, [
				"id" => md5($name . $artist),
				"img" => $imgUrl,
				"name" => $name,
				"artist" => $artist,
				"time" => $time,
				"now" => $now,
			]);
		}
	}

	return $tracks;
}
