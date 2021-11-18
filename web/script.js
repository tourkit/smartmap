
var ready = (callback) => { if (document.readyState != "loading") callback(); else document.addEventListener("DOMContentLoaded", callback) }

ready(() => { 

    menu = [

        { name:"Home", tree:[1,2,5,6]},
        { name:"Patch", tree:[7,8, 0]},
        { name:"Config", tree:[4,3,11, 0]},
        { name:"HDMI1", value:"WUXGA@50", tree:[12,13,  2]}, 
        { name:"Network", value:"127.0.0.1/24", tree:[9,10,2]},
        { name:"Backup", tree:[0]},
        { name:"Reset", tree:[0]},
        { name:"New", tree:[1]},
        { name:"Cluster 1", value:"1.001..1.028 @HDMI1", tree:[14,15,16,1]},
        { name:"Address", value:"127.0.0.1", tree:[4]},
        { name:"Mask", value:"24", tree:[4]},
        { name:"HDMI2", value:"WUXGA@50", tree:[12,13,2]},
        { name:"Resolution", value:"WUXGA", tree:[3]},
        { name:"Frequency", value:"50", tree:[3]},
        { name:"First address", value:"1.001", tree:[8]},
        { name:"Instances", value:"4", tree:[8]},
        { name:"Output", value:"HDMI1", tree:[8]},

    ];

    var cont = document.querySelector("#menu");

    function showMenu(x) {
 
        cont.textContent = "";

        for (i in menu[x].tree) {

            var id = menu[x].tree[i];

            var link =  document.createElement("div");
            link.id = id;
            link.className = "menuItem";
            link.textContent = menu[id].name;
            
            cont.appendChild(link);
            
            var value = document.createElement("div");
            value.className = "value";
            value.textContent = menu[id].value;
            link.appendChild(value);

            


            link.addEventListener("click", (e) => { showMenu(e.target.id); });
            

        }

    }

showMenu(0);



var str = "";


for (i = 0; i < 256; i++) str += '<ChannelSet index="'+i+'" name="Fx '+(i+1)+'" from_dmx="'+i+'" to_dmx="'+i+'" />\n';


var b = document.getElementsByTagName("body")[0];

b.textContent = str;

b.innerHTML  = b.innerHTML.replace(/\n/g, '<br>');
});
