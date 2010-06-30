<?php

class rssReader{

    public static $attributes = array( 
            'title','link','description','language','copyright','skipHours','timestamp',
            'managingEditor','webMaster','pubDate','lastBuildDate','category',
            'generator','docs','cloud','ttl','image','rating','textInput','skipDays');

    public static function feadToArray($urls)
    {
        $entries = array();

        // We support multiple urls at once. 
        if(!is_array($urls)) $urls = array($urls);

        foreach($urls as $url){ 
            $doc = new DOMDocument();
            $doc->load($url);
            foreach ($doc->getElementsByTagName('item') as $item) {

                // Collect data from feed
                $entry = array();
                $entry['url'] = $url;
                foreach(self::$attributes as $attr){
                    $entry[$attr] =NULL;
                    $obj = $item->getElementsByTagName($attr);
                    if(is_object($obj->item(0))){
                        $entry[$attr] = $obj->item(0)->nodeValue;
                    }
                }   

                // Fake timestamp in none is given.
                if($entry['pubDate'] == NULL){
                    $entry['timestamp'] = NULL;
                }else{

                    // Create an entry timestamp 
                    $entry['timestamp'] = strtotime($entry['pubDate']);
                }
                $entries[] = $entry;
            }
        }
        return($entries);
    }

    public static function sortFeedResultBy($feedRes , $sortBy = 'timestamp')
    {

        // Do not try to sort for invalid attributes.
        if(!in_array($sortBy, self::$attributes)){
            trigger_error("Invalid sortby attribute '{$sortBy}'!");
            return($feedRes);
        }

        // Prepare feeds to be sorted, put them in an array indexed by the 'sortBy' attribute.
        $data = array();
        foreach($feedRes as $feed){
            $key = "{$feed[$sortBy]}";
            $c = '1';
            while(empty($key) || isset($data[$key])){
                $key = "{$c}_{$feed[$sortBy]}";
                $c++;
            }
            $data[$key] = $feed;
        }

        // Sort the natural way and return the results.
        uksort($data, 'strnatcasecmp');
        return($data);
    }

} 

?>
