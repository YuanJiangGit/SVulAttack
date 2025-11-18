void SimulateRoutedMouseClickAt(WebContents* web_contents,
                                int modifiers,
                                blink::WebMouseEvent::Button button,
                                const gfx::Point& point) {
  content::WebContentsImpl* web_contents_impl =
      static_cast<content::WebContentsImpl*>(web_contents);
  content::RenderWidgetHostViewBase* rwhvb =
      static_cast<content::RenderWidgetHostViewBase*>(
          web_contents->GetRenderWidgetHostView());
  blink::WebMouseEvent mouse_event(blink::WebInputEvent::kMouseDown, modifiers,
                                   ui::EventTimeForNow());
  mouse_event.button = button;
  mouse_event.SetPositionInWidget(point.x(), point.y());
  gfx::Rect offset = web_contents->GetContainerBounds();
  mouse_event.SetPositionInScreen(point.x() + offset.x(),
                                  point.y() + offset.y());
  mouse_event.click_count = 1;
  web_contents_impl->GetInputEventRouter()->RouteMouseEvent(rwhvb, &mouse_event,
                                                            ui::LatencyInfo());
  mouse_event.SetType(blink::WebInputEvent::kMouseUp);
  web_contents_impl->GetInputEventRouter()->RouteMouseEvent(rwhvb, &mouse_event,
                                                            ui::LatencyInfo());
}
