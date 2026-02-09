<?php

$list = glob('instances/instance_xlsx/*.xlsx');
$index = '';

$setsConfiguration = file_get_contents("instances/dataSetsConfiguration.txt");
preg_match_all('#([^\s]*)\s(\d+\s\d+\s\d+\s\d+\s\d+)#is', $setsConfiguration, $matches);
$intances = $matches[1];
$sets = $matches[2];

foreach($list as $n => $file) {

    preg_match('#.*?/.*?/([^.]*)#is', $file, $match);
    $fileName = $match[1];

    preg_match('#([^_]*)#is', $fileName, $match);
    $name = $match[1];

    $key = array_search($name, $intances);

    print_r("{$file} -> instances/instance_txt/{$fileName}.txt\n");

    shell_exec("ssconvert {$file} instances/instance_txt/{$fileName}.txt");

    $txt = file_get_contents("instances/instance_txt/{$fileName}.txt");

    $txt = preg_replace('#,#is', ' ', $txt);

    $result = $sets[$key] . "\n" . $txt;

    file_put_contents("instances/instance_txt/{$fileName}.txt", $result);

    $index = $index . "instances/instance_txt/{$fileName}.txt\n";

}

file_put_contents('index.txt', $index);