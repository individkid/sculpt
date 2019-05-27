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
foreign import ccall clearDebug :: CInt -> IO ()
foreign import ccall enumerate :: Ptr CChar -> IO CInt
foreign import ccall rdPointer :: CInt -> IO (Ptr ())
foreign import ccall rdOpcode :: CInt -> IO CInt
foreign import ccall rdConfigure :: CInt -> IO CInt
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
   scriptOp :: CInt,
   windowOp :: CInt,
   fileOp :: CInt,
   planeOp :: CInt,
   confOp :: CInt,
   boundariesOp :: CInt,
   regionsOp :: CInt,
   planesOp :: CInt,
   sidesOp :: CInt,
   sideOp :: CInt,
   insidesOp :: CInt,
   outsidesOp :: CInt,
   insideOp :: CInt,
   outsideOp :: CInt,
   versorOp :: CInt,
   vectorOp :: CInt,
   delayOp :: CInt,
   countOp :: CInt,
   identOp :: CInt,
   valueOp :: CInt,
   -- scriptOp :: CInt,
   topologyOp :: CInt,
   fixedOp :: CInt,
   pierceOp :: CInt,
   seqnumOp :: CInt,
   matrixOp :: CInt,
   subconfOp :: CInt,
   settingOp :: CInt,
   textOp :: CInt,
   opcodes :: CInt}

data Configure = Configure {
   testConf :: CInt}

main = do
   print (holes 5 [2,3,4])
   setDebug (toCInt 0x10000)
   [rdfd,wrfd] <- fmap (map read) getArgs
   readOpV <- (newCString "ReadOp") >>= enumerate
   writeOpV <- (newCString "WriteOp") >>= enumerate
   scriptOpV <- (newCString "ScriptOp") >>= enumerate
   windowOpV <- (newCString "WindowOp") >>= enumerate
   fileOpV <- (newCString "FileOp") >>= enumerate
   planeOpV <- (newCString "PlaneOp") >>= enumerate
   confOpV <- (newCString "ConfOp") >>= enumerate
   boundariesOpV <- (newCString "BoundariesOp") >>= enumerate
   regionsOpV <- (newCString "RegionsOp") >>= enumerate
   planesOpV <- (newCString "PlanesOp") >>= enumerate
   sidesOpV <- (newCString "SidesOp") >>= enumerate
   sideOpV <- (newCString "SideOp") >>= enumerate
   insidesOpV <- (newCString "InsidesOp") >>= enumerate
   outsidesOpV <- (newCString "OutsidesOp") >>= enumerate
   insideOpV <- (newCString "InsideOp") >>= enumerate
   outsideOpV <- (newCString "OutsideOp") >>= enumerate
   versorOpV <- (newCString "VersorOp") >>= enumerate
   vectorOpV <- (newCString "VectorOp") >>= enumerate
   delayOpV <- (newCString "DelayOp") >>= enumerate
   countOpV <- (newCString "CountOp") >>= enumerate
   identOpV <- (newCString "IdentOp") >>= enumerate
   valueOpV <- (newCString "ValueOp") >>= enumerate
   -- scriptOpV <- (newCString "ScriptOp") >>= enumerate
   topologyOpV <- (newCString "TopologyOp") >>= enumerate
   fixedOpV <- (newCString "FixedOp") >>= enumerate
   pierceOpV <- (newCString "PierceOp") >>= enumerate
   seqnumOpV <- (newCString "SeqnumOp") >>= enumerate
   matrixOpV <- (newCString "MatrixOp") >>= enumerate
   subconfOpV <- (newCString "SubconfOp") >>= enumerate
   settingOpV <- (newCString "SettingOp") >>= enumerate
   textOpV <- (newCString "TextOp") >>= enumerate
   opcodesV <- (newCString "Opcodes") >>= enumerate
   testConfV <- (newCString "TestConf") >>= enumerate
   mainLoop rdfd wrfd Opcode {
   readOp = readOpV,
   writeOp = writeOpV,
   scriptOp = scriptOpV,
   windowOp = windowOpV,
   fileOp = fileOpV,
   planeOp = planeOpV,
   confOp = confOpV,
   boundariesOp = boundariesOpV,
   regionsOp = regionsOpV,
   planesOp = planesOpV,
   sidesOp = sidesOpV,
   sideOp = sideOpV,
   insidesOp = insidesOpV,
   outsidesOp = outsidesOpV,
   insideOp = insideOpV,
   outsideOp = outsideOpV,
   versorOp = versorOpV,
   vectorOp = vectorOpV,
   delayOp = delayOpV,
   countOp = countOpV,
   identOp = identOpV,
   valueOp = valueOpV,
   -- scriptOp = scriptOpV,
   topologyOp = topologyOpV,
   fixedOp = fixedOpV,
   pierceOp = pierceOpV,
   seqnumOp = seqnumOpV,
   matrixOp = matrixOpV,
   subconfOp = subconfOpV,
   settingOp = settingOpV,
   textOp = textOpV,
   opcodes = opcodesV} Configure {
   testConf = testConfV}

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
   | src == (readOp op) || src == (scriptOp op) = do
   ptr <- rdPointer rdfd
   exOpcode rdfd (fileOp op)
   file <- toInt (rdInt rdfd)
   exOpcode rdfd (planeOp op)
   plane <- toInt (rdInt rdfd)
   exOpcode rdfd (confOp op)
   conf <- rdConfigure rdfd
   dataIter rdfd wrfd op co conf
   wrOpcode wrfd src
   wrPointer wrfd ptr
mainIter _ _ _ _ _ = undefined

dataIter :: CInt -> CInt -> Opcode -> Configure -> CInt -> IO ()
dataIter rdfd wrfd op co conf
   | conf == (testConf co) = do
   exOpcode rdfd (textOp op)
   count <- toInt (rdInt rdfd)
   str <- newCString (replicate count '\0')
   rdChars rdfd (toCInt count) str
   (peekCString str) >>= print
dataIter _ _ _ _ _ = undefined
