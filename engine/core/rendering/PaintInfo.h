/*
 * Copyright (C) 2000 Lars Knoll (knoll@kde.org)
 *           (C) 2000 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 *           (C) 2004 Allan Sandfeld Jensen (kde@carewolf.com)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef SKY_ENGINE_CORE_RENDERING_PAINTINFO_H_
#define SKY_ENGINE_CORE_RENDERING_PAINTINFO_H_

#include <limits>
#include "sky/engine/core/rendering/PaintPhase.h"
#include "sky/engine/platform/geometry/IntRect.h"
#include "sky/engine/platform/geometry/LayoutRect.h"
#include "sky/engine/platform/graphics/GraphicsContext.h"
#include "sky/engine/platform/transforms/AffineTransform.h"
#include "sky/engine/wtf/HashMap.h"
#include "sky/engine/wtf/ListHashSet.h"

namespace blink {

class RenderInline;
class RenderLayerModelObject;
class RenderObject;

/*
 * Paint the object and its children, clipped by (x|y|w|h).
 * (tx|ty) is the calculated position of the parent
 */
struct PaintInfo {
    PaintInfo(GraphicsContext* newContext, const IntRect& newRect, PaintPhase newPhase,
        RenderObject* newPaintingRoot, const RenderLayerModelObject* newPaintContainer)
        : context(newContext)
        , rect(newRect)
        , phase(newPhase)
        , paintingRoot(newPaintingRoot)
        , m_paintContainer(newPaintContainer)
    {
    }

    void updatePaintingRootForChildren(const RenderObject* renderer)
    {
        if (!paintingRoot)
            return;

        // If we're the painting root, kids draw normally, and see root of 0.
        if (paintingRoot == renderer) {
            paintingRoot = 0;
            return;
        }
    }

    bool shouldPaintWithinRoot(const RenderObject* renderer) const
    {
        return !paintingRoot || paintingRoot == renderer;
    }

    void applyTransform(const AffineTransform& localToAncestorTransform, bool identityStatusUnknown = true)
    {
        if (identityStatusUnknown && localToAncestorTransform.isIdentity())
            return;

        context->concatCTM(localToAncestorTransform);

        if (rect == infiniteRect())
            return;

        if (localToAncestorTransform.isInvertible())
            rect = localToAncestorTransform.inverse().mapRect(rect);
        else
            rect.setSize(IntSize(0, 0));
    }

    static IntRect infiniteRect() { return IntRect(LayoutRect::infiniteRect()); }
    const RenderLayerModelObject* paintContainer() const { return m_paintContainer; }

    // FIXME: Introduce setters/getters at some point. Requires a lot of changes throughout rendering/.
    GraphicsContext* context;
    IntRect rect;
    PaintPhase phase;
    RenderObject* paintingRoot; // used to draw just one element and its visual kids

private:
    const RenderLayerModelObject* m_paintContainer; // the layer object that originates the current painting
};

} // namespace blink

#endif  // SKY_ENGINE_CORE_RENDERING_PAINTINFO_H_
