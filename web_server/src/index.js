const express = require("express");
const app = express();
const port = 3000;

app.get("/test", (req, res) => {
    console.log("Hello world!");
    res.send("Hello World!");
});

app.listen(port, () => {
    console.log(`Example app listening on port ${port}`)
});