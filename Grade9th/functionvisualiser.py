#I wrote this in 9th grade. No critique!
#As far as I remember this should run with python3.2 and pygame(unknown version)

from pygame import *
import pygame
import sys
import math



class Function():
    def __init__(self, func):
        self.func = func
        self.rules = list()
    def addrule(self, rule):
        self.rules.append(rule)
    def possible(self, x):
        for i in self.rules:
            if eval(i) == False:
                return False
        return True
    def evaluate(self, x):
        return eval(self.func)


class Coord():
    def __init__(self,x,y):
        self.x = x
        self.y = y
    def Print(self):
        print("[ " + str(self.x) + " , " + str(self.y) + " ]")
    def Normalize(self):
        l = self.leng()
        return Coord(self.x / l, self.y / l)
    def leng(self):
        return math.sqrt(self.x * self.x + self.y * self.y)
    def __add__(self, sec):
        return Coord(self.x + sec.x, self.y + sec.y)
    def __sub__(self, sec):
        return Coord(self.x - sec.x, self.y - sec.y)
    def __mul__(self, t):
        return Coord(self.x * t, self.y * t)
    def Lerp(self, sec, t):
        return self + (sec - self) * t
class Mat3x3():
    def Make(self, m11, m12, m13, m21, m22, m23, m31, m32, m33):
        self.m11, self.m12, self.m13, self.m21, self.m22, self.m23, self.m31, self.m32, self.m33 = m11, m12, m13, m21, m22, m23, m31, m32, m33
    def __init__(self):
        self.MakeIdentity()
    def Transform(self, coord):
        x = coord.x * self.m11 + coord.y * self.m21 + self.m31
        y = coord.x * self.m12 + coord.y * self.m22 + self.m32
        return Coord(x,y)
    def MakeIdentity(s):
        s.m11,s.m12,s.m13,s.m21,s.m22,s.m23,s.m31,s.m32,s.m33 = 1,0,0,0,1,0,0,0,1
    def Print(self):
        print("[ " + str(self.m11) + " , " + str(self.m12) + " , " + str(self.m13) + " ]")
        print("[ " + str(self.m21) + " , " + str(self.m22) + " , " + str(self.m23) + " ]")
        print("[ " + str(self.m31) + " , " + str(self.m32) + " , " + str(self.m33) + " ]")
class CoordSystemApp():
    def __init__(self):
        pygame.init()
        self.fpsClock = pygame.time.Clock()

        self.windowsize = Coord(640, 480)
        self.is_fullscreen = False
        
        self.windowSurfaceObj = pygame.display.set_mode((self.windowsize.x, self.windowsize.y))
        pygame.display.set_caption('Function Visualiser')

        self.redColor = pygame.Color(255,0,0)
        self.greenCOlor = pygame.Color(0,255,0)
        self.blueColor = pygame.Color(0,0,255)
        self.whiteColor = pygame.Color(255,255,255)
        self.greyColor = pygame.Color(175,175,175)
        self.darkGreyColor = pygame.Color(125, 125, 125)
        
        self.mousepos1 = Coord(0,0)
        self.mousepos2 = Coord(0,0)
        self.mousemotion = Coord(0,0)

        self.windowpos = Coord(0,0)
        
        self.fontObj = pygame.font.Font('freesansbold.ttf', 12)

        self.zoom = 1
        self.space = 35
        
        self.is_running = True
        self.is_dragging = False

        self.resolutions = [ Coord(640,480), Coord(1024, 768), Coord(1280,768), Coord(1360, 768), Coord(1366, 768) ]

        self.functions = list()
        self.is_editing = False
        self.is_typing = False
        self.text = ""
        self.selected = -1
        self.rule = -1
        self.shift = False

        self.showkeys = True
    def MainLoop(self):
        while True:
            self.HandleEvents()
            if self.is_running:
                self.Update()
                self.Render()
            else:
                break    
        pygame.quit()
    def UpdateDisplay(self):
        if self.is_fullscreen == True:
            pygame.display.set_mode((self.windowsize.x,self.windowsize.y),pygame.FULLSCREEN)
        else:
            pygame.display.set_mode((self.windowsize.x,self.windowsize.y))
    def HandleEvents(self):
        for event in pygame.event.get():
            if event.type == QUIT:
                self.is_running = False
                break
            elif event.type == MOUSEMOTION:
                self.mousepos2.x, self.mousepos2.y = event.pos
            elif event.type == MOUSEBUTTONUP:
                self.is_dragging = False
                self.mousepos2.x, self.mousepos2.y = event.pos
            elif event.type == MOUSEBUTTONDOWN:
                self.is_dragging = True
                self.mousepos2.x, self.mousepos2.y = event.pos
            else:
                if self.is_editing == True:
                    self.HandleManageEvents(event)
                else:
                    self.HandleActiveEvents(event)
    def HandleActiveEvents(self, event):
        if event.type == KEYDOWN:
            if event.key == K_w:
                if self.zoom < 1:
                    self.zoom += 0.1
                else:
                    self.zoom += 1;
                
            elif event.key == K_s:
                if self.zoom > 1:
                    self.zoom -= 1;
                elif self.zoom > 0.1:
                    self.zoom -= 0.1;
            elif event.key == K_f:
                    if self.is_fullscreen == False:
                        self.is_fullscreen = True
                    else:
                        self.is_fullscreen = False
                    self.UpdateDisplay()
            elif event.key == K_e:
                self.space += 1
            elif event.key == K_d:
                if self.space > 1:
                    self.space -= 1
            elif event.key == K_ESCAPE:
                    self.is_running = False
            elif event.key == K_h:
                if self.showkeys == True:
                    self.showkeys = False
                else:
                    self.showkeys = True
            elif event.key == K_r:
                a = -1
                for i in range(0, len(self.resolutions)):
                    if self.resolutions[i].x == self.windowsize.x:
                        if self.resolutions[i].y == self.windowsize.y:
                            a = i
                            break
                if a == len(self.resolutions) - 1:
                    a = 0
                else:
                    a += 1
                self.windowsize = self.resolutions[a]
                self.UpdateDisplay()
            elif event.key == K_m:
                self.is_editing = True
    def HandleManageEvents(self, event):
        if event.type == KEYUP:
            if event.key == K_LSHIFT:
                self.shift = False
        if event.type != KEYDOWN:
            return
        if self.is_typing == True:
            if event.key == K_RETURN:
                self.is_typing = False
                self.type_enter_clb(self.text)
                self.text = ""
            elif event.key == K_ESCAPE:
                self.text = ""
                self.is_typing = False
            elif event.key == K_SPACE:
                self.text += " "
            elif event.key == K_LSHIFT:
                self.shift = True
            elif event.key == 8: #clearing?
                self.text = self.text[:len(self.text) - 1]
            elif event.key == K_8:
                if self.shift == True:
                    self.text += "*"
                else:
                    self.text += "8"
            elif event.key == K_0:
                if self.shift == True:
                    self.text += ")"
                else:
                    self.text += "0"
            elif event.key == K_9:
                if self.shift == True:
                    self.text += "("
                else:
                    self.text += "9"
            elif event.key == K_COMMA:
                if self.shift == True:
                    self.text += "<"
                else:
                    self.text += ","
            elif event.key == K_PERIOD:
                if self.shift == True:
                    self.text += ">"
                else:
                    self.text += "."
            elif event.key == K_1:
                if self.shift == True:
                    self.text += "!"
                else:
                    self.text += "1"
            elif event.key == K_EQUALS:
                if self.shift == True:
                    self.text += "+"
                else:
                    self.text += "="
            else:
                self.text += str(chr(event.key))
        elif event.key == K_a:
            self.is_typing = True
            self.type_enter_clb = self.AddFunction
        elif event.key == K_r:
            if self.selected != -1:
                self.functions.pop(self.selected)
                if len(self.functions) - 1 < self.selected:
                    self.selected -= 1
        elif event.key == K_w:
            if self.selected > 0:
                self.selected -= 1
                if len(self.functions[self.selected].rules) > 0:
                    self.rule = 0
                else:
                    self.rule = -1
        elif event.key == K_s:
            if self.selected != len(self.functions) - 1 and self.selected != -1:
                self.selected += 1
                if len(self.functions[self.selected].rules) > 0:
                    self.rule = 0
                else:
                    self.rule = -1
        elif event.key == K_m:
            self.is_editing = False
        elif event.key == K_i:
            if self.rule > 0:
                self.rule -= 1
        elif event.key == K_k:
            if self.selected != -1:
                if self.rule != len(self.functions[self.selected].rules) - 1 and self.selected != -1:
                    self.rule += 1
        elif event.key == K_u:
            if self.selected != -1:
                self.is_typing = True
                self.type_enter_clb = self.AddRule
        elif event.key == K_j:
            if self.selected != -1:
                if self.rule != -1:
                    self.functions[self.selected].rules.pop(self.rule)
                    if len(self.functions[self.selected].rules) - 1 < self.rule:
                        self.rule -= 1
        elif event.key == K_h:
            if self.showkeys == True:
                self.showkeys = False
            else:
                self.showkeys = True
    def AddFunction(self, expr):
        self.functions.append(Function(expr))
        self.selected = len(self.functions) - 1
    def AddRule(self, rule):
        self.functions[self.selected].rules.append(rule)
        self.rule = len(self.functions[self.selected].rules) - 1
    def RenderGraph(self, fnc):
        x = self.windowpos.x
        pos1 = Coord(0,0)
        pos2 = Coord(0,0)
        s = ((self.windowpos.x * self.space / self.zoom) % self.space - self.space) / self.space * self.zoom
        x -= s
        while x < self.windowpos.x + self.windowsize.x / self.space * self.zoom:
            #get real coordinates
            x += self.zoom / 15
            if fnc.possible(x) == False:
                pos1.x = 0
                pos1.y = 0
                continue
            pos2 = Coord(x, fnc.evaluate(x))
            #transform them to screen coords
            pos2.x = (pos2.x - self.windowpos.x) * self.space / self.zoom
            pos2.y = (self.windowpos.y - pos2.y) * self.space / self.zoom
            if pos1.y > self.windowsize.y + 1000 or pos1.y < -1000:
                pos1.x = 0
                pos2.y = 0
                continue
            if pos2.y > self.windowsize.y + 1000 or pos2.y < -1000:
                pos1.x = 0
                pos1.y = 0
                continue
            #now render if we have updated pos1
            if pos1.x != 0 and pos1.y != 0:
                pygame.draw.line(self.windowSurfaceObj, self.redColor, (pos1.x, pos1.y), (pos2.x, pos2.y), 2)
            pos1 = pos2
    def RenderGrid(self):
        x = self.windowpos.x
        s = ((self.windowpos.x * self.space / self.zoom) % self.space - self.space) / self.space * self.zoom
        pos = int()
        while x < self.windowpos.x + self.windowsize.x // self.space * self.zoom:
            x+=self.zoom
            pos = (x - self.windowpos.x - s) * self.space / self.zoom
            pygame.draw.line(self.windowSurfaceObj, self.greyColor, (pos, 0), (pos, self.windowsize.y - 40), 2)
            if round( x - s, 1)  == 0.0:
                pygame.draw.line(self.windowSurfaceObj, self.darkGreyColor, (pos, 0), (pos, self.windowsize.y - 40), 3)
            txtsur = self.fontObj.render(str( float( round( x - s , 2)) ) ,False,self.greyColor)
            txtrect = txtsur.get_rect()
            txtrect.topleft = (pos, self.windowsize.y - 20)
            self.windowSurfaceObj.blit(txtsur, txtrect)

        y = self.windowpos.y - self.windowsize.y // self.space * self.zoom - self.zoom
        y -= self.zoom
        y += 40 // self.space * self.zoom
        s = ((self.windowpos.y * self.space / self.zoom) % self.space - self.space) / self.space * self.zoom
        while y < self.windowpos.y + self.windowsize.y // self.space * self.zoom:
            y+=self.zoom
            pos = (self.windowpos.y - y + s) * self.space / self.zoom
            pygame.draw.line(self.windowSurfaceObj, self.greyColor, (40, pos), (self.windowsize.x, pos), 2)
            if round( y - s, 1)  == 0.0:
                pygame.draw.line(self.windowSurfaceObj, self.darkGreyColor, (40, pos), (self.windowsize.x, pos), 3)
            txtsur = self.fontObj.render(str( float( round( y - s, 2 ) ) ),False,self.greyColor)
            txtrect = txtsur.get_rect()
            txtrect.topleft = (2, pos)
            self.windowSurfaceObj.blit(txtsur, txtrect)
        #draw possible actions
        x = self.windowsize.x - 200
        if self.showkeys == True:
            self.RenderText("W -> Size between lines + 1", Coord(x, 30), self.redColor)
            self.RenderText("S -> Size between lines - 1", Coord(x, 50), self.redColor)
            self.RenderText("E -> Zoom + 1", Coord(x, 70), self.redColor)
            self.RenderText("D -> Zoom - 1", Coord(x, 90), self.redColor)
            self.RenderText("R -> Change The Resolution", Coord(x, 110), self.redColor)
            self.RenderText("F -> Fullscreen/Windowed", Coord(x, 130), self.redColor)
            self.RenderText("M -> Manage Functions", Coord(x, 150), self.redColor)
            self.RenderText("H -> Hide this text", Coord(x, 170), self.redColor)
            self.RenderText("Copyright 2012 - Nikolay Dionisov - All rights reserved!", Coord(x- 125,self.windowsize.y - 50), self.redColor)
        self.RenderText("FPS: " + str(self.fpsClock.get_fps()), Coord(x, 10), self.redColor)
        #self.RenderText("R"
    def RenderText(self, txt, pos, clr):
        txtsur = self.fontObj.render((txt),False,clr)
        txtrect = txtsur.get_rect()
        txtrect.topleft = (pos.x, pos.y)
        self.windowSurfaceObj.blit(txtsur, txtrect)
    def RenderGUI(self):
        pygame.draw.rect(self.windowSurfaceObj, self.greyColor, (80,40,200,300), 0)
        x = self.windowsize.x - 280
        pygame.draw.rect(self.windowSurfaceObj, self.greyColor, (x, 40, 200, 300), 0)
        pygame.draw.rect(self.windowSurfaceObj, self.whiteColor, (90, 50, 180, 20), 0)
        pygame.draw.rect(self.windowSurfaceObj, self.whiteColor, (x + 10, 50, 180, 20), 0)
        txtsur = self.fontObj.render("Functions", False, self.greyColor)
        txtrect = txtsur.get_rect()
        txtrect.topleft = (100, 55)
        self.windowSurfaceObj.blit(txtsur, txtrect)
        txtsur = self.fontObj.render("Rules", False, self.greyColor)
        txtrect = txtsur.get_rect()
        txtrect.topleft = (x + 20, 55)
        self.windowSurfaceObj.blit(txtsur, txtrect)

        px = 0
        for i in range(0, len(self.functions)):
            color = pygame.Color(200,200,200)
            if self.selected == i:
                color = pygame.Color(255,70,0)
            pygame.draw.rect(self.windowSurfaceObj, color, (90, 80 + px, 180, 15), 0)
            txtsur = self.fontObj.render(self.functions[i].func, False, self.whiteColor)
            txtrect = txtsur.get_rect()
            txtrect.topleft = (90, 80 + px)
            self.windowSurfaceObj.blit(txtsur, txtrect)
            px+=20
        if self.selected != -1:
            px = 0
            for i in range(0, len(self.functions[self.selected].rules)):
                color = pygame.Color(200,200,200)
                if self.rule == i:
                    color = pygame.Color(255,70,0)
                pygame.draw.rect(self.windowSurfaceObj, color, (x + 10, 80 + px, 180, 15), 0)
                txtsur = self.fontObj.render(self.functions[self.selected].rules[i], False, self.whiteColor)
                txtrect = txtsur.get_rect()
                txtrect.topleft = (x + 10, 80 + px)
                self.windowSurfaceObj.blit(txtsur, txtrect)
                px+=20

        txtsur = self.fontObj.render(self.text, False, self.blueColor)
        txtrect = txtsur.get_rect()
        txtrect.topleft = (90, 350)
        self.windowSurfaceObj.blit(txtsur, txtrect)

        #render keys
        if self.showkeys == True:
            x = self.windowsize.x - 220
            y = self.windowsize.y - 250
            self.RenderText("A -> Add a function",Coord(x, y),self.redColor)
            self.RenderText("Note: Start writing it right", Coord(x, y + 20),self.redColor)
            self.RenderText("after you have clicked A", Coord(x, y + 40),self.redColor)
            self.RenderText("R -> Remove the current f()", Coord(x, y + 60),self.redColor)
            self.RenderText("W -> select the f() above", Coord(x, y + 80),self.redColor)
            self.RenderText("S -> select the f() below", Coord(x, y + 100),self.redColor)
            self.RenderText("I -> select the f() rule above", Coord(x, y + 120),self.redColor)
            self.RenderText("K -> select the f() rule below", Coord(x, y + 140),self.redColor)
            self.RenderText("U -> add a new rule to the selected f()", Coord(x, y + 160),self.redColor)
            self.RenderText("J -> remove the selected rule", Coord(x, y + 180),self.redColor)
            self.RenderText("M -> Go to the board", Coord(x, y + 200),self.redColor)
            self.RenderText("H -> Hide this text", Coord(x, y + 220),self.redColor)
    def Render(self):
        self.windowSurfaceObj.fill(self.whiteColor)
        if self.is_editing == True:
            self.RenderGUI()
        else:
            self.RenderGrid()
            for i in self.functions:
                self.RenderGraph(i)
        pygame.display.update()
    def Update(self):
        if self.is_dragging:
            self.mousemotion.x = self.mousepos2.x - self.mousepos1.x
            self.mousemotion.y = self.mousepos2.y - self.mousepos1.y
            self.windowpos.x -= self.mousemotion.x / self.space * self.zoom
            self.windowpos.y += self.mousemotion.y / self.space * self.zoom
            #move.Print()
        self.mousepos1 = Coord(self.mousepos2.x, self.mousepos2.y)
        self.fpsClock.tick(120)
app = CoordSystemApp()
app.MainLoop()
