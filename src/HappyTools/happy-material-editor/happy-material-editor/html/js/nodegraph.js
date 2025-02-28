function LOG(str)
{
    console.log(str);
}


$(function()
{
    // Globals
    var g_Nodes = new Array();
    var g_Graph = $("#graph");
    var g_Canvas = $("#canvas");
    var g_ZIndex = 0;
    var g_ShiftPressed = false;
    var g_CtrlPressed = false;
    var g_DragHelper;
    var g_LastClickedNode;
    var g_MovingConnection;

    // Init
    Init();

    function Init()
    {
        // Resize graph with document
        $(window).resize(function(ev)
        {
            LOG(ev);
            Resize();
        });
        // Do initial sizing
        Resize();

        InitSelecting();
        
        for (var x=0; x < 5; ++x)
        for (var y=0; y < 2; ++y)
        {
            var node1 = new Node("TestName", 64 + 150 * x, 64 + 80 * y);
            node1.AddConnector("right", "R", true);
            node1.AddConnector("right", "G", true);
            node1.AddConnector("right", "B", true);
            node1.AddConnector("right", "A", true);
            node1.AddConnector("left", "RGB", false);
            node1.AddConnector("left", "RGBA", false);
            node1.AddConnector("top", "A", false);
            node1.AddConnector("bottom", "B", true);
        }

        Redraw();
    }

    function Resize()
    {
        var win = $( window );
        var w = win.width();
        var h = win.height();
        g_Graph.css(
        {
            "width" : w,
            "height" : h
        });
        g_Canvas.css(
        {
            "width" : w,
            "height" : h
        });
        var c = document.getElementById("canvas");
        c.width = w;
        c.height = h;
        Redraw();
    }

    function NodeSortFunc(a, b)
    {
        return a.m_Element.zIndex() - b.m_Element.zIndex();
    }

    function SortNodes()
    {
        g_Nodes.sort(NodeSortFunc);
    }

    function Redraw()
    {
        var c = document.getElementById("canvas");
        var ctx = c.getContext("2d");

        ctx.clearRect(0,0,c.width,c.height)

        for (var i in g_Nodes)
        {
            var node = g_Nodes[i];
            node.Draw(ctx);
        }
    }

    function InitSelecting()
    {
        g_Graph.selectable(
        {
            filter : ".node",
            selecting: Redraw,
            unselecting: Redraw,
            start: Redraw,
            stop: Redraw
        });

    }

    function SelectNode(node, deselect)
    {        
        // add unselecting class to all elements in the styleboard canvas except the ones to select
        if (deselect)
            $(".ui-selected").not(node).removeClass("ui-selected").addClass("ui-unselecting");

        // add ui-selecting class to the elements to select
        node.not(".ui-selected").addClass("ui-selecting");

        // trigger the mouse stop event (this will select all .ui-selecting elements, and deselect all .ui-unselecting elements)
        g_Graph.selectable('refresh');
        g_Graph.data("ui-selectable")._mouseStop(null);
    }

    // Dragging helper
    function GetContainmentNode()
    {
        var selected = $(".ui-selected");
        
        g_Graph.append("<div id='containmentNode'>Hello!</div>");
        var container = $("#containmentNode");


        var left = 10000000;
        var top = 10000000;
        var right = 0;
        var bottom = 0;

        selected.each(function(index, element)
        {
            var el = $(element);
            left = Math.min(left, el.position().left);
            top = Math.min(top, el.position().top);
            right = Math.max(right, el.width() + el.position().left);
            bottom = Math.max(bottom, el.height() + el.position().top);
        });

        var width = right - left;
        var height = bottom - top;

        container.css(
        {
            "position" : "absolute",
            "left" : left, "top" : top,
            "width" : width, "height" : height,
            "background-color" : "white",
            "padding" : "0px",
            "margin" : "0px",
        });

        // Make sure the offset is correct when multidragging
        var offsetX = container.position().left - g_LastClickedNode.position().left;
        var offsetY = container.position().top - g_LastClickedNode.position().top;
        container.css("left", "+="+offsetX);
        container.css("top", "+="+offsetY);

        // add selected elements to helper
        selected.each(function(index, element)
        {
            var el = $(element);
            el.detach().appendTo(container);
            el.css("left", el.position().left - left);
            el.css("top", el.position().top - top);
        });

        g_DragHelper = container;

        return container;
    }

    function IsSelected(node)
    {
        return $(node).hasClass("ui-selected");
    }

    function GetPosition(element)
    {
        var pos = new float2(0, 0);
        if (element != undefined)
        {
            var off = element.offset();
            pos.left = off.left;
            pos.top = off.top;
        }
        return pos;
    }

    function Node(name, x, y)
    {
        // CTOR
        this.m_Name = name;
        this.m_Id = g_Nodes.length;
        this.m_Connectors = { "left" : new Array(), "top" : new Array(), "right" : new Array(), "bottom" : new Array() };
        this.m_IsMoving = false;

        g_Graph.append("<div class='node'>Hello!</div>");
        this.m_Element = $(".node").last();
        this.m_Element.css(
        {
            "position" : "absolute",
            "left" : x, "top" : y,
            "width" : "128px", "height" : "64px",
            "border" : "1px solid gray",
            "background-color" : "white"
        });
        this.m_Element.mousedown(function(ev)
        {
            var deselect = IsSelected(this) == false && ev.shiftKey == false;
            SelectNode($(this), deselect);
            g_LastClickedNode = $(this);
            return false;
        });
        this.m_Element.click(function(ev)
        {
            var deselect = ev.shiftKey == false;
            SelectNode($(this), deselect);
        });
        this.m_Element.draggable(
        { 
            stack: ".node", 
            containment: g_Graph,
            distance:2,
            helper: function() { return GetContainmentNode(); },
            start: function(event, ui)
            {
                SortNodes();
                Redraw();
            },
            drag: function(event, ui)
            {
                Redraw();
            },
            stop: function(event, ui)
            {
                var selected = $(".ui-selected");
                selected.each(function(index, element)
                {
                    var el = $(element);
                    el.detach().appendTo(g_Graph);
                    el.css("left", el.position().left + g_DragHelper.position().left);
                    el.css("top", el.position().top + g_DragHelper.position().top);
                    
                });
                g_DragHelper = undefined;

                Redraw();
            }
        });

        g_Nodes.push(this);
        //////////////////////

        // pos = left, ip, right, down
        this.AddConnector = function(pos, name, provider)
        {
            this.m_Connectors[pos].push(new Connector(this, pos, provider, name));
            this.RepositionConnectors(pos);
        };

        this.RepositionConnectors = function(pos)
        {
            var isLeft = pos == "left";
            var isRight = pos == "right";
            var isTop = pos == "top";
            var isBottom = pos == "bottom";

            var startX = 0;
            var startY = 0;
            var maxX = 0;
            var maxY = 0;

            var self = this.m_Element;

            if (isLeft || isRight)
            {
                maxX = 0;
                maxY = self.height();
                if (isLeft)
                    startX = 0;
                else
                    startX = self.width();
                startY = -4;
            }
            else
            {
                maxX = self.width();
                maxY = 0;
                startX = -4;
                if (isTop)
                    startY = 0;
                else
                    startY = self.height();
            }

            var connectors = this.m_Connectors[pos];
            var count = connectors.length;
            var offsetX = maxX / (count + 1);
            var offsetY = maxY / (count + 1);
            for (var i in connectors)
            {
                var el = connectors[i];
                startX += offsetX;
                startY += offsetY;
                el.m_Element.css(
                {
                    "left": startX,
                    "top": startY
                });
            }
        };
        this.GetSelectState = function()
        {
            var state = 0;
            if (this.m_Element.hasClass("ui-selected"))
                state = 1;
            else if (this.m_Element.hasClass("ui-selecting"))
                state = 2;
            else if (this.m_Element.hasClass("ui-unselecting"))
                state = 3;
            return state;
        };
        this.Draw = function(ctx)
        {
            var selectState = this.GetSelectState();
            switch (selectState)
            {
                case 0: ctx.fillStyle = "#FF0000"; break;
                case 1: ctx.fillStyle = "#00FF00"; break;
                case 2: ctx.fillStyle = "#FFFF00"; break;
                case 3: ctx.fillStyle = "#999900"; break;
            }
                
            var position = GetPosition(this.m_Element);
            var x = position.left;
            var y = position.top;
            ctx.fillRect(x,y,128,64);

            ctx.fillStyle = "#000000";
            ctx.font = "16px OpenSansRegular";
            ctx.textAlign = "center";
            ctx.fillText(this.m_Name, x+64, y+32);

            for (var i in this.m_Connectors)
            {
                var connectorList = this.m_Connectors[i];
                for (j in connectorList)
                {
                    var connector = connectorList[j];
                    connector.Draw(ctx);
                }
            }
        };

    }

    function GetOppositeSide(side)
    {
        if (side == "left") return "right";
        if (side == "right") return "left";
        if (side == "top") return "bottom";
        if (side == "bottom") return "top";
        return "left";
    }

    function GetHelperConnection(from)
    {
        this.m_From = from;
        this.m_FallbackSide = GetOppositeSide(from.m_Side);
        this.m_ConnectedTo = undefined;

        g_Graph.append("<div class='connector'>"+name+"</div>");
        this.m_Element = $(".connector").last();
        this.m_Element.css(
        {
            "position" : "absolute",
            "width" : "8px", "height" : "8px",
            "background-color" : "black"
        });

        this.Draw = function(ctx)
        {
            var connectPos = GetPosition(this.m_Element);
            var connectDir = this.m_FallbackSide;
            if (this.m_ConnectedTo != undefined)
            {
                if (this.m_ConnectedTo.CheckInside(connectPos) == false)
                    this.m_ConnectedTo = undefined;
            }
            if (this.m_ConnectedTo == undefined)
            {
                for (var i in g_Nodes)
                {
                    var node = g_Nodes[i];
                    if (node != this.m_Parent)
                    {
                        for (j in node.m_Connectors)
                        {
                            var connectorList = node.m_Connectors[j];
                            for (k in connectorList)
                            {
                                var otherConnector = connectorList[k];
                                if (this.m_From.m_Provider != otherConnector.m_Provider && otherConnector.CheckInside(connectPos))
                                {
                                    this.m_ConnectedTo = otherConnector;
                                    break;
                                }
                            }
                        }
                    }
                    if (this.m_ConnectedTo != undefined)
                    {
                        break;
                    }
                }
            }
            if (this.m_ConnectedTo != undefined)
            {
                connectPos = GetPosition(this.m_ConnectedTo.m_Element);
                connectDir = this.m_ConnectedTo.m_Side;
            }

            var position = GetPosition(this.m_From.m_Element);
            DrawConnection(ctx, position, this.m_From.m_Side, connectPos, connectDir);

            ctx.fillStyle = "#0000FF";
            ctx.fillRect(connectPos.left, connectPos.right, 8, 8);        
        };
    }

    function ConnectionModSide(side, pos, diff)
    {
        if (side === "left") pos.left -= Math.max(32, Math.abs(diff.left));
        else if (side === "top") pos.top -= Math.max(32, Math.abs(diff.top));
        else if (side === "right") pos.left += Math.max(32, Math.abs(diff.left));
        else if (side === "bottom") pos.top += Math.max(32, Math.abs(diff.top));        
    }

    function DrawConnection(ctx, posFrom, sideFrom, posTo, sideTo)
    {
        var diff = new float2(posTo.left, posTo.top);
        diff.Sub(posFrom);
        diff.Mul(0.75);

        var cp1 = new float2(posFrom.left, posFrom.top);
        ConnectionModSide(sideFrom, cp1, diff);

        var cp2 = new float2(posTo.left, posTo.top);
        ConnectionModSide(sideTo, cp2, diff);

        // Curve
        ctx.beginPath();
        ctx.moveTo(posFrom.left, posFrom.top);
        ctx.bezierCurveTo(cp1.left, cp1.top, cp2.left, cp2.top, posTo.left, posTo.top);
        ctx.lineWidth = 4;
        ctx.strokeStyle = 'black';
        ctx.stroke();
    }

    function Connection(provider, receiver)
    {
        this.m_Provider = provider;
        this.m_Receiver = receiver;

        provider.AddConnection(this);
        receiver.AddConnection(this);

        this.Destroy = function()
        {
            this.m_Provider.RemoveConnection(this);
            this.m_Receiver.RemoveConnection(this);
        };

        this.Draw = function(ctx)
        {
            var posFrom = GetPosition(this.m_Provider.m_Element);
            var posTo = GetPosition(this.m_Receiver.m_Element);
            DrawConnection(ctx, posFrom, this.m_Provider.m_Side, posTo, this.m_Receiver.m_Side);
        };
    }

    function Connector(parent, side, provider, name)
    {
        this,m_Self = this;
        this.m_DragHelper = undefined;
        this.m_Side = side;
        this.m_Provider = provider;
        this.m_Connections = new Array();

        var parentNode = parent.m_Element;
        parentNode.append("<div class='connector'>"+name+"</div>");
        this.m_Element = $(".connector").last();

        this.AddConnection = function(connection)
        {
            if (this.m_Provider == false)
            {
                // Can only contain 1 connection
                if (this.m_Connections.length > 0)
                {
                    this.m_Connections[0].Destroy();
                }
            }
            this.m_Connections.push(connection);
        };

        this.RemoveConnection = function(connection)
        {
            var size = this.m_Connections.length;
            for (var i = 0; i < size; ++i)
            {
                if (this.m_Connections[i] === connection)
                {
                    this.m_Connections.splice(i, 1);
                    break;
                }
            }
        };

        this.StopDragging = function()
        {
            var connectedTo = this.m_DragHelper.m_ConnectedTo;
            if (connectedTo != undefined)
            {
                var provider = this;
                var receiver = this;
                if (this.m_Provider)
                    receiver = connectedTo;
                else
                    provider = connectedTo;
                var connection = new Connection(provider, receiver);
            }
            this.m_DragHelper = undefined;
            Redraw();
        };

        this.GetHelperConnector = function()
        {
            this.m_DragHelper = new GetHelperConnection(this);
            return this.m_DragHelper.m_Element;
        };

        this.m_Element.css(
        {
            "position" : "absolute",
            "width" : "8px", "height" : "8px",
            "background-color" : "black"
        });
        this.m_Element.draggable(
        {
            containment: g_Graph,
            helper: function (self) { return function(event, ui) { return self.GetHelperConnector(); }; }(this),
            drag: function(event, ui) { Redraw(); },
            stop: function (self) { return function(event, ui) { return self.StopDragging(); }; }(this),
        });

        this.CheckInside = function(pos)
        {
            var myPos = GetPosition(this.m_Element);
            var xDiff = pos.left - myPos.left;
            var yDiff = pos.top - myPos.top;
            var lenSq = xDiff * xDiff + yDiff * yDiff;
            return lenSq < 8 * 8;
        }

        this.Draw = function(ctx)
        {
            ctx.fillStyle = "#000000";
            var position = GetPosition(this.m_Element);
            ctx.beginPath();
            ctx.arc(position.left + 4, position.top + 4, 4, 0, Math.PI * 2);
            ctx.fill();

            if (this.m_DragHelper != undefined)
            {
                this.m_DragHelper.Draw(ctx);
            }    

            if (this.m_Provider)
            {
                for (var i in this.m_Connections)
                {
                    this.m_Connections[i].Draw(ctx);
                }
            }
        };
    }

});
