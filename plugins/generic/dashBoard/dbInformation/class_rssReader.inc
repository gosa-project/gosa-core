<?php

class rssReader{

    static function feadToArray($url)
    {

        $doc = new DOMDocument();
        $doc->load($url);
        $entries = array();
        foreach ($doc->getElementsByTagName('item') as $item) {
            $entry = array ( 
                    'title' => $item->getElementsByTagName('title')->item(0)->nodeValue,
                    'desc' => $item->getElementsByTagName('description')->item(0)->nodeValue,
                    'link' => $item->getElementsByTagName('link')->item(0)->nodeValue,
                    'date' => $item->getElementsByTagName('pubDate')->item(0)->nodeValue,
                    'timestamp' => strtotime($item->getElementsByTagName('pubDate')->item(0)->nodeValue)
                    );
            $entries[$entry['timestamp']."-".$entry['date']] = $entry;
        }
        return($entries);
    }
} 

?>
