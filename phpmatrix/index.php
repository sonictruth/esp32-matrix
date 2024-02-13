<?
$customScrollMessagefile = "message.txt";
$slogansFile = "slogans.txt";

$currentScrollMessage = file_get_contents($customScrollMessagefile);
$currentSlogans = file_get_contents($slogansFile);

if ($_POST['t']) {
    $currentScrollMessage = trim($_POST['t']);
    $currentSlogans = trim($_POST['s']);
    if ($_POST['p'] === "stereop") {
        file_put_contents($customScrollMessagefile, $currentScrollMessage);
        file_put_contents($slogansFile, $currentSlogans);

        echo "<h1 class='error'>Saved!</h1>";
    } else {
        echo "<h1  class='error'>Wrong password!</h1>";
    }
}

?>
<html>

<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

    <!-- Bootstrap CSS -->
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/css/bootstrap.min.css"
        integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous">

    <title>Matrix</title>
    <style>
        .error {
            text-align: center;
        }

        textarea {
            white-space: pre;
            overflow-wrap: normal;
            overflow-x: scroll;
        }
    </style>
</head>

<body>
    <div class="container">
        <h1>Custom message</h1>
        <form method="POST" action="?">
            <label for="t">Password</label>
            <div class="form-group">
                <input class="form-control" name="p" type="password">
            </div>
            <label for="t">Custom Scroll Message</label>
            <div class="form-group">
                <textarea style="height: 60px;" name="t" class="form-control"><?= htmlentities($currentScrollMessage) ?></textarea>
            </div>

            <label for="s">Slogans (new line = new slogan)</label>
            <div class="form-group">
                <textarea style="height: 200px;" name="s"
                    class="form-control"><?= htmlentities($currentSlogans) ?></textarea>
            </div>

            <div class="form-group">
                <input class="btn btn-primary" type="submit" value="Update" />
            </div>
        </form>
    </div>
</body>

</html>