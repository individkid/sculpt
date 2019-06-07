--    Main interface through C to another process
--    Copyright (C) 2016  Paul Coelho
--
--    This program is free software: you can redistribute it and/or modify
--    it under the terms of the GNU General Public License as published by
--    the Free Software Foundation, either version 3 of the License, or
--    (at your option) any later version.
--
--    This program is distributed in the hope that it will be useful,
--    but WITHOUT ANY WARRANTY; without even the implied warranty of
--    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--    GNU General Public License for more details.
--
--    You should have received a copy of the GNU General Public License
--    along with this program.  If not, see <http://www.gnu.org/licenses/>.

{-# LANGUAGE ForeignFunctionInterface #-}

module Main where

import Foreign.C
import Foreign.Ptr
import System.Environment
import AffTopo.Naive

foreign import ccall setDebug :: CInt -> IO ()
foreign import ccall enumerate :: Ptr CChar -> IO CInt
foreign import ccall rdPointer :: CInt -> IO (Ptr ())
foreign import ccall rdOpcode :: CInt -> IO CInt
foreign import ccall rdConfigure :: CInt -> IO CInt
foreign import ccall rdSculpt :: CInt -> IO CInt
foreign import ccall rdClickMode :: CInt -> IO CInt
foreign import ccall rdMouseMode :: CInt -> IO CInt
foreign import ccall rdRollerMode :: CInt -> IO CInt
foreign import ccall rdTargetMode :: CInt -> IO CInt
foreign import ccall rdTopologyMode :: CInt -> IO CInt
foreign import ccall rdFixedMode :: CInt -> IO CInt
foreign import ccall rdSubconf :: CInt -> IO CInt
foreign import ccall rdInt :: CInt -> IO CInt
foreign import ccall rdFloat :: CInt -> IO CFloat
foreign import ccall rdDouble :: CInt -> IO CDouble
foreign import ccall rdChars :: CInt -> CInt -> Ptr CChar -> IO ()
foreign import ccall rdInts :: CInt -> CInt -> Ptr CInt -> IO ()
foreign import ccall rdFloats :: CInt -> CInt -> Ptr CFloat -> IO ()
foreign import ccall rdDoubles :: CInt -> CInt -> Ptr CDouble -> IO ()
foreign import ccall wrPointer :: CInt -> Ptr () -> IO ()
foreign import ccall wrOpcode :: CInt -> CInt -> IO ()
foreign import ccall wrConfigure :: CInt -> CInt -> IO ()
foreign import ccall wrSculpt :: CInt -> CInt -> IO CInt
foreign import ccall wrClickMode :: CInt -> CInt -> IO CInt
foreign import ccall wrMouseMode :: CInt -> CInt -> IO CInt
foreign import ccall wrRollerMode :: CInt -> CInt -> IO CInt
foreign import ccall wrTargetMode :: CInt -> CInt -> IO CInt
foreign import ccall wrTopologyMode :: CInt -> CInt -> IO ()
foreign import ccall wrFixedMode :: CInt -> CInt -> IO ()
foreign import ccall wrSubconf :: CInt -> CInt -> IO ()
foreign import ccall wrInt :: CInt -> CInt -> IO ()
foreign import ccall wrFloat :: CInt -> CFloat -> IO ()
foreign import ccall wrDouble :: CInt -> CDouble -> IO ()
foreign import ccall wrChars :: CInt -> CInt -> Ptr CChar -> IO ()
foreign import ccall wrInts :: CInt -> CInt -> Ptr CInt -> IO ()
foreign import ccall wrFloats :: CInt -> CInt -> Ptr CFloat -> IO ()
foreign import ccall wrDoubles :: CInt -> CInt -> Ptr CDouble -> IO ()
foreign import ccall exOpcode :: CInt -> CInt -> IO ()
foreign import ccall hello :: CString -> IO CString

data Opcode = Opcode {
   readOp :: CInt,
   writeOp :: CInt,
   windowOp :: CInt,
   fileOp :: CInt,
   planeOp :: CInt,
   confOp :: CInt,
   sculptOp :: CInt,
   clickOp :: CInt,
   mouseOp :: CInt,
   rollerOp :: CInt,
   targetOp :: CInt,
   topologyOp :: CInt,
   fixedOp :: CInt,
   boundariesOp :: CInt,
   regionsOp :: CInt,
   planesOp :: CInt,
   sidesOp :: CInt,
   sideOp :: CInt,
   insidesOp :: CInt,
   outsidesOp :: CInt,
   insideOp :: CInt,
   outsideOp :: CInt,
   matrixOp :: CInt,
   versorOp :: CInt,
   vectorOp :: CInt,
   delayOp :: CInt,
   countOp :: CInt,
   identOp :: CInt,
   valueOp :: CInt,
   metricOp :: CInt,
   subconfOp :: CInt,
   settingOp :: CInt,
   filenameOp :: CInt,
   scriptOp :: CInt,
   queryOp :: CInt,
   displayOp :: CInt,
   textOp :: CInt,
   manipOp :: CInt,
   commandOp :: CInt,
   pointerOp :: CInt}

data Configure = Configure {}

main = do
   print (holes 5 [2,3,4])
   setDebug (toCInt 1)
   [rdfd,wrfd] <- fmap (map read) getArgs
   readOpV <- (newCString "ReadOp") >>= enumerate
   writeOpV <- (newCString "WriteOp") >>= enumerate
   windowOpV <- (newCString "WindowOp") >>= enumerate
   fileOpV <- (newCString "FileOp") >>= enumerate
   planeOpV <- (newCString "PlaneOp") >>= enumerate
   confOpV <- (newCString "ConfOp") >>= enumerate
   sculptOpV <- (newCString "SculptOp") >>= enumerate
   clickOpV <- (newCString "ClickOp") >>= enumerate
   mouseOpV <- (newCString "MouseOp") >>= enumerate
   rollerOpV <- (newCString "RollerOp") >>= enumerate
   targetOpV <- (newCString "TargetOp") >>= enumerate
   topologyOpV <- (newCString "TopologyOp") >>= enumerate
   fixedOpV <- (newCString "FixedOp") >>= enumerate
   boundariesOpV <- (newCString "BoundariesOp") >>= enumerate
   regionsOpV <- (newCString "RegionsOp") >>= enumerate
   planesOpV <- (newCString "PlanesOp") >>= enumerate
   sidesOpV <- (newCString "SidesOp") >>= enumerate
   sideOpV <- (newCString "SideOp") >>= enumerate
   insidesOpV <- (newCString "InsidesOp") >>= enumerate
   outsidesOpV <- (newCString "OutsidesOp") >>= enumerate
   insideOpV <- (newCString "InsideOp") >>= enumerate
   outsideOpV <- (newCString "OutsideOp") >>= enumerate
   matrixOpV <- (newCString "MatrixOp") >>= enumerate
   versorOpV <- (newCString "VersorOp") >>= enumerate
   vectorOpV <- (newCString "VectorOp") >>= enumerate
   delayOpV <- (newCString "DelayOp") >>= enumerate
   countOpV <- (newCString "CountOp") >>= enumerate
   identOpV <- (newCString "IdentOp") >>= enumerate
   valueOpV <- (newCString "ValueOp") >>= enumerate
   metricOpV <- (newCString "MetricOp") >>= enumerate
   subconfOpV <- (newCString "SubconfOp") >>= enumerate
   settingOpV <- (newCString "SettingOp") >>= enumerate
   filenameOpV <- (newCString "FilenameOp") >>= enumerate
   scriptOpV <- (newCString "ScriptOp") >>= enumerate
   queryOpV <- (newCString "QueryOp") >>= enumerate
   displayOpV <- (newCString "DisplayOp") >>= enumerate
   textOpV <- (newCString "TextOp") >>= enumerate
   manipOpV <- (newCString "ManipOp") >>= enumerate
   commandOpV <- (newCString "CommandOp") >>= enumerate
   pointerOpV <- (newCString "PointerOp") >>= enumerate
   mainLoop rdfd wrfd Opcode {
   readOp = readOpV,
   writeOp = writeOpV,
   windowOp = windowOpV,
   fileOp = fileOpV,
   planeOp = planeOpV,
   confOp = confOpV,
   sculptOp = sculptOpV,
   clickOp = clickOpV,
   mouseOp = mouseOpV,
   rollerOp = rollerOpV,
   targetOp = targetOpV,
   topologyOp = topologyOpV,
   fixedOp = fixedOpV,
   boundariesOp = boundariesOpV,
   regionsOp = regionsOpV,
   planesOp = planesOpV,
   sidesOp = sidesOpV,
   sideOp = sideOpV,
   insidesOp = insidesOpV,
   outsidesOp = outsidesOpV,
   insideOp = insideOpV,
   outsideOp = outsideOpV,
   matrixOp = matrixOpV,
   versorOp = versorOpV,
   vectorOp = vectorOpV,
   delayOp = delayOpV,
   countOp = countOpV,
   identOp = identOpV,
   valueOp = valueOpV,
   metricOp = metricOpV,
   subconfOp = subconfOpV,
   settingOp = settingOpV,
   filenameOp = filenameOpV,
   scriptOp = scriptOpV,
   queryOp = queryOpV,
   displayOp = displayOpV,
   textOp = textOpV,
   manipOp = manipOpV,
   commandOp = commandOpV,
   pointerOp = pointerOpV} Configure {}

toInt :: Integral a => IO a -> IO Int
toInt = fmap (fromInteger . toInteger)

toCInt :: Integral a => a -> CInt
toCInt a = fromInteger (toInteger a)

mainLoop :: CInt -> CInt -> Opcode -> Configure -> IO ()
mainLoop rdfd wrfd op co = do
   src <- rdOpcode rdfd
   mainIter rdfd wrfd op co src
   mainLoop rdfd wrfd op co

mainIter :: CInt -> CInt -> Opcode -> Configure -> CInt -> IO ()
mainIter rdfd wrfd op co src
   | src == (displayOp op) = do
   ptr <- rdPointer rdfd
   exOpcode rdfd (fileOp op)
   file <- toInt (rdInt rdfd)
   queryIter rdfd wrfd file op co
   wrOpcode wrfd src
   wrPointer wrfd ptr
   | otherwise = undefined

queryIter :: CInt -> CInt -> Int -> Opcode -> Configure -> IO ()
queryIter rdfd wrfd file op co = do
   exOpcode rdfd (textOp op)
   count <- toInt (rdInt rdfd)
   str <- newCString (replicate count '\0')
   rdChars rdfd (toCInt count) str
   (peekCString str) >>= print
